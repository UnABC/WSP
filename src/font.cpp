//#define GL_GLEXT_PROTOTYPES
#include <GL/glew.h>
#include "font.hpp"
using namespace std;

Font::~Font() {
	//フォントの解放
	if (textureArrayID != 0)glDeleteTextures(1, &textureArrayID);
	if (vbo)glDeleteBuffers(1, &vbo);
	if (vao)glDeleteVertexArrays(1, &vao);
	if (face)FT_Done_Face(face);
	if (library)FT_Done_FreeType(library);
}

void Font::Init(int width, int height) {
	if (FT_Init_FreeType(&library))
		throw FontException("Failed to initialize FreeType library.");
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	//頂点バッファ(posX, posY, texX, texY,layerIndex, color)
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 9, nullptr, GL_DYNAMIC_DRAW);

	// 位置属性 (vec3)
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// テクスチャ座標属性 (vec2)
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	// レイヤーインデックス属性 (float)
	glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(5 * sizeof(float)));
	glEnableVertexAttribArray(2);
	// 色属性 (vec3)
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(3);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	//shaderProgram = ShaderUtil::createShaderProgram(vertexShaderSource, fragmentShaderSource);
	projection = ShaderUtil::recalcProjection(width, height);
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
	if (FT_Set_Pixel_Sizes(face, 0, size))
		throw FontException("Failed to set font size.");
	FT_Select_Charmap(face, FT_ENCODING_UNICODE);
	slot = face->glyph;
	font_size = size;

	//よく使われる文字のキャッシュを作成
	if (textureArrayID != 0) {
		glDeleteTextures(1, &textureArrayID);
		textureArrayID = 0;
	}
	characters.clear();
	nextLayerIndex = 0;
	maxGlyphWidth = size;
	maxGlyphHeight = size;
	glGenTextures(1, &textureArrayID);
	glBindTexture(GL_TEXTURE_2D_ARRAY, textureArrayID);

	glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RED, maxGlyphWidth, maxGlyphHeight, maxTextureSize, 0, GL_RED, GL_UNSIGNED_BYTE, nullptr);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	// ASCII文字のキャッシュを作成
	for (char16_t c = 32; c < 127;c++) {
		if (FT_Load_Glyph(face, FT_Get_Char_Index(face, c), FT_LOAD_RENDER))
			throw FontException("Failed to load character.");
		glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, nextLayerIndex, slot->bitmap.width, slot->bitmap.rows, 1, GL_RED, GL_UNSIGNED_BYTE, slot->bitmap.buffer);
		Character character_data;
		character_data.LayerIndex = nextLayerIndex;
		character_data.Size = { (float)slot->bitmap.width, (float)slot->bitmap.rows };
		character_data.Bearing = { (float)slot->bitmap_left, (float)slot->bitmap_top };
		character_data.Advance = (float)(slot->advance.x >> 6);
		characters[c] = character_data;
		nextLayerIndex++;
	}
	glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

	if (shaderProgram) glDeleteProgram(shaderProgram);
	shaderProgram = ShaderUtil::createShaderProgram(vertexShaderSource, fragmentShaderSource);
}

void Font::SetTexts(string text, float x, float y, float scale, int width, SDL_Color color1, SDL_Color color2, SDL_Color color3, SDL_Color color4, float depth) {
	if (!face || !shaderProgram || !vbo || !vao)
		throw FontException("Font not initialized.");
	glUseProgram(shaderProgram);
	//表示座標の計算
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D_ARRAY, textureArrayID);
	glUniform1i(glGetUniformLocation(shaderProgram, "textTextureArray"), 0);
	glBindVertexArray(vao);
	//glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	double current_x = x;
	double current_y = y;
	FT_Long line_height_pixels = face->size->metrics.height >> 6;

	u16string text16str = boost::locale::conv::utf_to_utf<char16_t>(text);
	const char16_t* text16 = text16str.c_str();

	int all_vertices_size = text16str.size() * 6 * 9;
	int before_size = all_vertices.size();
	all_vertices.resize(before_size + all_vertices_size);

	struct normalized_color {
		float r, g, b;
	} normalized_color1 = { color1.r / 255.0f, color1.g / 255.0f, color1.b / 255.0f };
	struct normalized_color normalized_color2 = { color2.r / 255.0f, color2.g / 255.0f, color2.b / 255.0f };
	struct normalized_color normalized_color3 = { color3.r / 255.0f, color3.g / 255.0f, color3.b / 255.0f };
	struct normalized_color normalized_color4 = { color4.r / 255.0f, color4.g / 255.0f, color4.b / 255.0f };

	for (size_t i = 0; text16[i] != u'\0'; i++) {
		if (text16[i] == '\n') {
			current_x = x;
			current_y += line_height_pixels * scale;
			continue;
		}
		//キャッシュ作成
		if (!characters.count(text16[i])) {
			bool is_overflow = false;
			if (nextLayerIndex >= maxTextureSize) {
				is_overflow = true;
				nextLayerIndex = (characters.begin()->second).LayerIndex;
				characters.erase(begin(characters));
			}
			glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
			glBindTexture(GL_TEXTURE_2D_ARRAY, textureArrayID);
			if (FT_Load_Glyph(face, FT_Get_Char_Index(face, text16[i]), FT_LOAD_RENDER))
				throw FontException("Failed to load character.");
			glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, nextLayerIndex, slot->bitmap.width, slot->bitmap.rows, 1, GL_RED, GL_UNSIGNED_BYTE, slot->bitmap.buffer);
			Character character_data;
			character_data.LayerIndex = nextLayerIndex;
			character_data.Size = { (float)slot->bitmap.width, (float)slot->bitmap.rows };
			character_data.Bearing = { (float)slot->bitmap_left, (float)slot->bitmap_top };
			character_data.Advance = (float)(slot->advance.x >> 6);
			characters[text16[i]] = character_data;
			if (is_overflow) {
				nextLayerIndex = maxTextureSize;
			} else {
				nextLayerIndex++;
			}
			glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
		}
		Character ch = characters[text16[i]];
		float render_x = current_x + ch.Bearing.x * scale;
		float render_y = current_y + (font_size - ch.Bearing.y) * scale;
		float glyph_width = ch.Size.x * scale;
		float glyph_height = ch.Size.y * scale;

		float tex_u_max = (ch.Size.x > 0 && maxGlyphWidth > 0) ? (float)ch.Size.x / maxGlyphWidth : 0.0f;
		float tex_v_max = (ch.Size.y > 0 && maxGlyphHeight > 0) ? (float)ch.Size.y / maxGlyphHeight : 0.0f;
		float current_layer_idx = (float)ch.LayerIndex;

		float vertices[6][9] = { // posX, posY, texU, texV, layerIndex,color
			{ render_x              , render_y + glyph_height,depth, 0.0f     , tex_v_max, current_layer_idx,normalized_color4.r,normalized_color4.g,normalized_color4.b },
			{ render_x              , render_y               ,depth, 0.0f     , 0.0f     , current_layer_idx,normalized_color1.r,normalized_color1.g,normalized_color1.b },
			{ render_x + glyph_width, render_y               ,depth, tex_u_max, 0.0f     , current_layer_idx,normalized_color2.r,normalized_color2.g,normalized_color2.b },

			{ render_x              , render_y + glyph_height,depth, 0.0f     , tex_v_max, current_layer_idx,normalized_color4.r,normalized_color4.g,normalized_color4.b },
			{ render_x + glyph_width, render_y               ,depth, tex_u_max, 0.0f     , current_layer_idx,normalized_color2.r,normalized_color2.g,normalized_color2.b },
			{ render_x + glyph_width, render_y + glyph_height,depth, tex_u_max, tex_v_max, current_layer_idx,normalized_color3.r,normalized_color3.g,normalized_color3.b },
		};
		// 頂点データをall_verticesに追加
		memcpy(&all_vertices[before_size + i * 6 * 9], vertices, sizeof(vertices));
		current_x += ch.Advance * scale;
	}
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * all_vertices.size(), all_vertices.data(), GL_DYNAMIC_DRAW);
}

void Font::DrawTexts() {
	if (all_vertices.empty())return;
	if (!shaderProgram || !vbo || !vao)
		throw FontException("Font not initialized.");
	glUseProgram(shaderProgram);
	glBindVertexArray(vao);
	glDrawArrays(GL_TRIANGLES, 0, (all_vertices.size() / 9));
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Font::Clear() {
	all_vertices.clear();
}

