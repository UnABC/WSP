#ifndef FONT_HPP
#define FONT_HPP

#include <ft2build.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_opengl.h>
#include <SDL3/SDL_opengl_glext.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "exception.hpp"
#include "shader.hpp"
#include <map>
#include <boost/locale.hpp>
#include FT_FREETYPE_H

struct Character {
	GLuint TextureID;	//グリフのテクスチャID
	float *vertices[6][4];	//グリフの頂点情報
};

class Font {
private:
	FT_Library library;
	FT_Face face;
	FT_GlyphSlot slot;
	glm::mat4 projection;
	GLuint vao, vbo;
	const char* vertexShaderSource = R"glsl(
		#version 450 core
		layout (location = 0) in vec4 vertex; // vec2 pos, vec2 tex
		out vec2 TexCoords;
		uniform mat4 projection;
		void main() {
			gl_Position = projection * vec4(vertex.xy, 0.0, 1.0);
			TexCoords = vertex.zw;
		}
		)glsl";

	const char* fragmentShaderSource = R"glsl(
		#version 450 core
		in vec2 TexCoords;
		out vec4 color;
		uniform sampler2D text;
		uniform vec3 textColor;
		void main()
		{    
			vec4 sampled = vec4(1.0, 1.0, 1.0, texture(text, TexCoords).r);
			color = vec4(textColor, 1.0) * sampled;
		}
		)glsl";
	GLuint shaderProgram;

	//フォントのキャッシュ
	std::map<char16_t, Character> characters;

	void CreateCharacters(const char* text);
public:
	~Font();
	void Init(int width, int height);
	void SetFont(const char* font_path, int size);
	void recalcProjection(int width, int height) { projection = glm::ortho(0.0f, (float)width, (float)height, 0.0f, -1.0f, 1.0f);glViewport(0, 0, width, height); }
	void DrawTexts(std::string text, float x, float y, float scale, SDL_Color color, int width);
};



#endif