#ifndef IMAGE_HPP
#define IMAGE_HPP
#include <SDL3/SDL.h>
#include <SDL3/SDL_opengl.h>
#include <map>
#include "shader.hpp"
#include <stb_image.h>

struct image_data {
	int width, height, channels, center_x, center_y;
	unsigned int texture;
};

class Image {
private:
	int width, height;
	glm::mat4 projection;

	std::map<unsigned int, image_data> images; // 画像データのリスト
	std::vector<float> all_image_vertices; // すべての画像の頂点を格納するベクター

	GLuint vao, vbo;
	GLuint shaderProgram;
	const char* vertexShaderSource = R"glsl(
		#version 330 core
		layout (location = 0) in vec3 position;
		layout (location = 1) in vec2 texCoord;
		layout (location = 2) in vec3 color;
		out vec3 outColor;
		out vec2 TexCoords;
		uniform mat4 projection;
		uniform mat4 transform;
		void main() {
			gl_Position = projection * transform * vec4(position, 1.0);
			outColor = color;
			TexCoords = texCoord;
		}
	)glsl";
	const char* fragmentShaderSource = R"glsl(
		#version 330 core
		out vec4 FragColor;
		in vec2 TexCoords;
		in vec3 outColor;
		uniform sampler2D image;
		void main() {
			FragColor = texture(image, TexCoords) * vec4(outColor, 1.0);
		}
	)glsl";
public:
	void Init(int width, int height);
	void Load(const std::string& file_path, int id, int center_x = 0, int center_y = 0);
	void DrawImage(unsigned int id, float x, float y, float x_size, float y_size, float angle, SDL_Color color1, SDL_Color color2, SDL_Color color3, SDL_Color color4, float depth);

	void Draw();
	void Clear() {
		all_image_vertices.clear();
	};
};

#endif