#ifndef SHAPE_HPP
#define SHAPE_HPP
#include <SDL3/SDL.h>
#include <SDL3/SDL_opengl.h>
#include "shader.hpp"
#include <vector>

class Shape {
	int width, height;
	GLuint vao, vbo, shaderProgram;
	// すべての頂点を格納するベクター
	std::vector<float> all_vertices;

	glm::mat4 projection;
	const char* vertexShaderSource = R"glsl(
		#version 330 core
		layout (location = 0) in vec3 position;
		layout (location = 1) in vec3 color;
		out vec3 outColor;
		uniform mat4 projection;
		void main() {
			gl_Position = projection * vec4(position, 1.0);
			outColor = color;
		}
	)glsl";
	const char* fragmentShaderSource = R"glsl(
		#version 330 core
		out vec4 FragColor;
		in vec3 outColor;
		void main() {
			FragColor = vec4(outColor, 1.0);
		}
	)glsl";

public:
	Shape() : width(640), height(480), vao(0), vbo(0), shaderProgram(0) {};
	~Shape();
	void Init(int w, int h);
	void draw_triangle(double x1, double y1, double x2, double y2, double x3, double y3, SDL_Color color1,SDL_Color color2, SDL_Color color3,float depth);

	void draw_shapes();
	void Clear() { all_vertices.clear(); };
};

#endif