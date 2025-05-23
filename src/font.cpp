//#define GL_GLEXT_PROTOTYPES
#include <GL/glew.h>
#include "font.hpp"
using namespace std;

Font::~Font() {
	//フォントの解放
	for (auto& character : characters)
		glDeleteTextures(1, &character.second.TextureID);
	if (vbo)glDeleteBuffers(1, &vbo);
	if (vao)glDeleteVertexArrays(1, &vao);
	if (face)FT_Done_Face(face);
	if (library)FT_Done_FreeType(library);
}

void Font::Init(int width, int height) {
	glewExperimental = GL_TRUE;
	GLenum err = glewInit();
	if (err != GLEW_OK)
		throw FontException(reinterpret_cast<const char*>(glewGetErrorString(err)));
	if (FT_Init_FreeType(&library))
		throw FontException("Failed to initialize FreeType library.");
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, nullptr, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	shaderProgram = ShaderUtil::createShaderProgram(vertexShaderSource, fragmentShaderSource);
	recalcProjection(width, height);
}

void Font::CreateCharacters(const char* text) {
	//フォントのキャッシュを作成
	/*for (int i = 0;text[i] != '\0';i++) {
		if (characters.count(text[i])) continue;
		FT_Load_Glyph(face, FT_Get_Char_Index(face, text[i]), FT_LOAD_RENDER);
		GLuint texture;
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, slot->bitmap.width, slot->bitmap.rows, 0, GL_RED, GL_UNSIGNED_BYTE, slot->bitmap.buffer);

		glBindTexture(texture, 0);

		characters[text[i]] = { texture,{ (float)slot->bitmap.width,(float)slot->bitmap.rows },{ (float)slot->bitmap_left,(float)slot->bitmap_top },(GLuint)(slot->advance.x) };
	}
	glBindTexture(GL_TEXTURE_2D, 0);*/
}

void Font::SetFont(const char* font_path, int size) {
	//フォントの初期化
	//if (face) FT_Done_Face(face);
	if (FT_New_Face(library, font_path, 0, &face))
		throw FontException("Failed to load font.");
	if (FT_Set_Pixel_Sizes(face, 0, size))
		throw FontException("Failed to set font size.");
	FT_Select_Charmap(face, FT_ENCODING_UNICODE);
	slot = face->glyph;
}

void Font::DrawTexts(string text, float x, float y, float scale, SDL_Color color, int width) {
	if (!face || !shaderProgram || !vbo || !vao) {
		throw FontException("Font not initialized.");
	}
	glUseProgram(shaderProgram);
	//色の設定
	glUniform3f(glGetUniformLocation(shaderProgram, "textColor"), color.r, color.g, color.b);
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
	glActiveTexture(GL_TEXTURE0);
	glUniform1i(glGetUniformLocation(shaderProgram, "text"), 0);
	glBindVertexArray(vao);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	double current_x = x;
	double current_y = y;
	FT_Long line_height_pixels = face->size->metrics.height >> 6;

	u16string text16str = boost::locale::conv::utf_to_utf<char16_t>(text);
	const char16_t* text16 = text16str.c_str();
	for (size_t i = 0; text16[i] != u'\0'; i++) {
		if (text16[i] == '\n') {
			current_x = x;
			current_y += line_height_pixels * scale;
			continue;
		}
		if (!characters.count(text16[i])) {
			if (FT_Load_Glyph(face, FT_Get_Char_Index(face, text16[i]), FT_LOAD_RENDER))
				throw FontException("Failed to load character.");
			GLuint glyph_texture_id;
			glGenTextures(1, &glyph_texture_id);
			glBindTexture(GL_TEXTURE_2D, glyph_texture_id);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, slot->bitmap.width, slot->bitmap.rows, 0, GL_RED, GL_UNSIGNED_BYTE, slot->bitmap.buffer);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			float render_x = current_x + slot->bitmap_left;
			float render_y = current_y - slot->bitmap_top;
			float glyph_width = slot->bitmap.width;
			float glyph_height = slot->bitmap.rows;

			float vertices[6][4] = {
				{ render_x, render_y + glyph_height, 0.0f, 1.0f }, { render_x, render_y, 0.0f, 0.0f }, { render_x + glyph_width, render_y, 1.0f, 0.0f },
				{ render_x, render_y + glyph_height, 0.0f, 1.0f }, { render_x + glyph_width, render_y, 1.0f, 0.0f }, { render_x + glyph_width, render_y + glyph_height, 1.0f, 1.0f }
			};
			characters[text16[i]] = { glyph_texture_id,*vertices };
		}
		GLint glyph_texture_id = characters[text16[i]].TextureID;
		float vertices[6][4];
		memccpy(vertices, characters[text16[i]].vertices, sizeof(float) * 6 * 4, sizeof(float) * 6 * 4);
		glBindTexture(GL_TEXTURE_2D, glyph_texture_id);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		//glDeleteTextures(1, &glyph_texture_id);
		current_x += (slot->advance.x >> 6);
	}
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
}

