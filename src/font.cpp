//#define GL_GLEXT_PROTOTYPES
#include <GL/glew.h>
#include "font.hpp"
using namespace std;

Font::~Font() {
	//フォントの解放
	if (vbo)glDeleteBuffers(1, &vbo);
	if (vao)glDeleteVertexArrays(1, &vao);
	if (face)FT_Done_Face(face);
	if (library)FT_Done_FreeType(library);
}

void Font::Init(unordered_map<char16_t, Character>& global_char, BLTexture& global_texture, glm::mat4* proj, glm::mat4* global_view) {
	projection = proj;
	view = global_view;

	if (FT_Init_FreeType(&library))
		throw FontException("Failed to initialize FreeType library.");
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	//頂点バッファ(posX, posY, texX, texY,layerIndex, color)
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 11, nullptr, GL_DYNAMIC_DRAW);
	// 位置属性 (vec3)
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// テクスチャ座標属性 (vec2)
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	// テクスチャハンドル属性 (uvec2)
	glVertexAttribIPointer(2, 2, GL_UNSIGNED_INT, 11 * sizeof(float), (void*)(5 * sizeof(float)));
	glEnableVertexAttribArray(2);
	// 色属性 (vec4)
	glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(7 * sizeof(float)));
	glEnableVertexAttribArray(3);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	//shaderProgram = ShaderUtil::createShaderProgram(vertexShaderSource, fragmentShaderSource);

	characters = &global_char;
	glyph_atlas = &global_texture;
}

void Font::MakeCache(char16_t c) {
	if (characters->count(c)) return; // 既にキャッシュが存在する場合は何もしない

	if (FT_Load_Char(face, c, FT_LOAD_RENDER))
		throw FontException("Failed to load character.");

	FT_GlyphSlot glyph = face->glyph;
	const unsigned int pedding = 2;

	if (atlas_cursor.x + glyph->bitmap.width + pedding > atlas_max_size) {
		atlas_cursor.x = 0;
		atlas_cursor.y += atlas_line_height;
		atlas_line_height = 0;
	}
	// オーバーしたら初期化
	if (atlas_cursor.y + glyph->bitmap.rows + pedding > atlas_max_size) {
		glyph_atlas->create(atlas_max_size, atlas_max_size, GL_R8, GL_RED);
		atlas_cursor = { 0, 0 };
		atlas_line_height = 0;
		characters->clear();
	}
	glyph_atlas->update_texture(atlas_cursor.x, atlas_cursor.y, glyph->bitmap.width, glyph->bitmap.rows, slot->bitmap.buffer, GL_RED);

	Character character_data;
	character_data.Size = { (float)slot->bitmap.width, (float)slot->bitmap.rows };
	character_data.Bearing = { (float)slot->bitmap_left, (float)slot->bitmap_top };
	character_data.Advance = (float)(slot->advance.x >> 6);
	character_data.uv_top_left = { (float)atlas_cursor.x / atlas_max_size, (float)atlas_cursor.y / atlas_max_size };
	character_data.uv_bottom_right = { (float)(atlas_cursor.x + slot->bitmap.width) / atlas_max_size, (float)(atlas_cursor.y + slot->bitmap.rows) / atlas_max_size };
	(*characters)[c] = character_data;

	atlas_cursor.x += glyph->bitmap.width + pedding;
	atlas_line_height = max(atlas_line_height, (unsigned int)glyph->bitmap.rows + pedding);
}

void Font::SetFont(const char* font_path, int size) {
	if (old_font_path == font_path && old_font_size == size) return;
	old_font_path = font_path;
	old_font_size = size;
	//フォントの初期化
	if (face) {
		FT_Done_Face(face);
		face = nullptr;
	}
	if (FT_New_Face(library, font_path, 0, &face))
		throw FontException("Failed to load font.");
	if (FT_Set_Pixel_Sizes(face, 0, max(size, 23)))
		throw FontException("Failed to set font size.");
	FT_Select_Charmap(face, FT_ENCODING_UNICODE);
	slot = face->glyph;
	font_size = size;

	//よく使われる文字のキャッシュを作成
	characters->clear();
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	// ASCII文字のキャッシュを作成
	for (char16_t c = 32; c < 127;c++)
		MakeCache(c);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

	if (shaderProgram) glDeleteProgram(shaderProgram);
	shaderProgram = ShaderUtil::createShaderProgram(vertexShaderSource, fragmentShaderSource);
}

void Font::SetTexts(string text, float x, float y, int width, SDL_Color color1, SDL_Color color2, SDL_Color color3, SDL_Color color4, int gmode, vector<AllVertexData>& all_vertices) {
	if (!face || !shaderProgram || !vbo || !vao)
		throw FontException("Font not initialized.");
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	//表示座標の計算
	double current_x = x;
	double current_y = y;
	FT_Long line_height_pixels = face->size->metrics.height >> 6;

	u16string text16str = boost::locale::conv::utf_to_utf<char16_t>(text);
	const char16_t* text16 = text16str.c_str();

	struct normalized_color {
		float r, g, b, a;
	} normalized_color1 = { color1.r / 255.0f, color1.g / 255.0f, color1.b / 255.0f, color1.a / 255.0f };
	struct normalized_color normalized_color2 = { color2.r / 255.0f, color2.g / 255.0f, color2.b / 255.0f, color2.a / 255.0f };
	struct normalized_color normalized_color3 = { color3.r / 255.0f, color3.g / 255.0f, color3.b / 255.0f, color3.a / 255.0f };
	struct normalized_color normalized_color4 = { color4.r / 255.0f, color4.g / 255.0f, color4.b / 255.0f, color4.a / 255.0f };

	float scale = 1.0f;
	if (font_size < 23)
		scale *= (float)font_size / 23.0f; // フォントサイズが小さい場合はスケールを調整

	for (size_t i = 0; text16[i] != u'\0'; i++) {
		if (text16[i] == '\n') {
			current_x = x;
			current_y += line_height_pixels * scale;
			continue;
		}
		//キャッシュ作成
		if (!characters->count(text16[i]))
			MakeCache(text16[i]);
		Character ch = (*characters)[text16[i]];

		GLuint64 handle = glyph_atlas->getBindlessTextureHandle();
		GLuint handle1 = static_cast<GLuint>(handle & 0xFFFFFFFF);
		GLuint handle2 = static_cast<GLuint>(handle >> 32);

		// 座標の計算
		float xpos = current_x + ch.Bearing.x * scale;
		float ypos = current_y + (font_size - ch.Bearing.y * scale);
		float w = ch.Size.x * scale;
		float h = ch.Size.y * scale;

		glm::vec3 v_bottom_left = { xpos    , ypos,     0.0f };
		glm::vec3 v_bottom_right = { xpos + w, ypos,     0.0f };
		glm::vec3 v_top_left = { xpos    , ypos + h, 0.0f };
		glm::vec3 v_top_right = { xpos + w, ypos + h, 0.0f };

		glm::vec2 uv_bl = { ch.uv_top_left.x,     ch.uv_bottom_right.y };
		glm::vec2 uv_br = { ch.uv_bottom_right.x, ch.uv_bottom_right.y };
		glm::vec2 uv_tl = ch.uv_top_left;
		glm::vec2 uv_tr = ch.uv_bottom_right;

		float tex_u_max = ch.uv_bottom_right.x - ch.uv_top_left.x;
		float tex_v_max = ch.uv_bottom_right.y - ch.uv_top_left.y;

		float vertices[6][11] = { // posX, posY, texU, texV, layerIndex,color
			{ v_top_left.x    , v_top_left.y    , v_top_left.z    , uv_tl.x, uv_bl.y, *(float*)&handle1,*(float*)&handle2,normalized_color4.r,normalized_color4.g,normalized_color4.b,normalized_color4.a },
			{ v_bottom_left.x , v_bottom_left.y , v_bottom_left.z , uv_bl.x, uv_tl.y, *(float*)&handle1,*(float*)&handle2,normalized_color1.r,normalized_color1.g,normalized_color1.b,normalized_color1.a },
			{ v_bottom_right.x, v_bottom_right.y, v_bottom_right.z, uv_br.x, uv_tl.y, *(float*)&handle1,*(float*)&handle2,normalized_color2.r,normalized_color2.g,normalized_color2.b,normalized_color2.a },

			{ v_top_left.x    , v_top_left.y    , v_top_left.z    , uv_tl.x, uv_bl.y, *(float*)&handle1,*(float*)&handle2,normalized_color4.r,normalized_color4.g,normalized_color4.b,normalized_color4.a },
			{ v_bottom_right.x, v_bottom_right.y, v_bottom_right.z, uv_br.x, uv_tl.y, *(float*)&handle1,*(float*)&handle2,normalized_color2.r,normalized_color2.g,normalized_color2.b,normalized_color2.a },
			{ v_top_right.x   , v_top_right.y   , v_top_right.z   , uv_tr.x, uv_bl.y, *(float*)&handle1,*(float*)&handle2,normalized_color3.r,normalized_color3.g,normalized_color3.b,normalized_color3.a },
		};
		// 頂点データをall_verticesに追加
		int local_gmode = gmode - (gmode % 2);
		if (all_vertices.empty() || (all_vertices.back().gmode != local_gmode) || (all_vertices.back().ID != 0)) {
			AllVertexData new_data;
			new_data.all_vertices = vector<float>();
			new_data.gmode = local_gmode;
			new_data.ID = 0; // 0: テキスト
			new_data.projection = *projection;
			new_data.view = *view;
			new_data.vao = vao;
			new_data.vbo = vbo;
			new_data.shaderProgram = shaderProgram;
			new_data.division = 11;
			all_vertices.push_back(new_data);
		}
		// 既存の頂点データに新しい頂点を追加
		all_vertices.back().all_vertices.insert(all_vertices.back().all_vertices.end(), &vertices[0][0], &vertices[0][0] + 6 * 11);
		current_x += ch.Advance * scale;
	}
}
