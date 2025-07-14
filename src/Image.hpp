#ifndef IMAGE_HPP
#define IMAGE_HPP
#include "shader.hpp"
#include "BLTexture.hpp"
#include <SDL3/SDL.h>
#include <SDL3/SDL_opengl.h>
#include <map>

struct image_data {
	int width, height, center_x, center_y;
	BLTexture texture; // Bindless texture for OpenGL
};

class Image {
private:
	glm::mat4 *projection;

	// 画像データのリスト
	std::map<unsigned int, image_data> *images;

	GLuint vao, vbo;
	GLuint shaderProgram;
	const char* vertexShaderSource = R"glsl(
		#version 450 core
		layout (location = 0) in vec3 position;
		layout (location = 1) in vec2 texCoord;
		layout (location = 2) in vec4 color;
		layout (location = 3) in uvec2 aTexHandle;
		layout (location = 4) in mat4 transform;
		out vec4 outColor;
		out vec2 TexCoords;
		flat out uvec2 TexHandle;

		uniform mat4 projection;
		void main() {
			gl_Position = projection * vec4(position, 1.0);
			outColor = color;
			TexCoords = texCoord;
			TexHandle = uvec2(aTexHandle);
		}
	)glsl";
	const char* fragmentShaderSource = R"glsl(
		#version 450 core
		#extension GL_ARB_bindless_texture : require
		out vec4 FragColor;
		in vec2 TexCoords;
		in vec4 outColor;
		flat in uvec2 TexHandle;
		void main() {
			sampler2D image = sampler2D(TexHandle);
			FragColor = texture(image, TexCoords) * outColor;
		}
	)glsl";
public:
	void Init(std::map<unsigned int, image_data> &global_images, glm::mat4 *proj);
	void Load(const std::string& file_path, int id, int center_x = 0, int center_y = 0);
	void DrawImage(unsigned int id, float x, float y, float x_size, float y_size, float angle, int tex_x, int tex_y, int tex_width, int tex_height, SDL_Color color1, SDL_Color color2, SDL_Color color3, SDL_Color color4, int gmode, std::vector<AllVertexData>& all_vertices);
};

#endif