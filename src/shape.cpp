#include <GL/glew.h>
#include "shape.hpp"
using namespace std;

Shape::~Shape() {
	// OpenGLのリソースを解放
	if (vao) glDeleteVertexArrays(1, &vao);
	if (vbo) glDeleteBuffers(1, &vbo);
	if (shaderProgram) glDeleteProgram(shaderProgram);
}

void Shape::Init(int w, int h) {
	width = w;
	height = h;
	// OpenGLの初期化
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	// 頂点バッファを初期化(posX,posY,color)
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * 6, nullptr, GL_DYNAMIC_DRAW);
	// 位置属性 (vec3)
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// 色属性 (vec3)
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	// シェーダープログラムの作成
	shaderProgram = ShaderUtil::createShaderProgram(vertexShaderSource, fragmentShaderSource);
	if (!shaderProgram)
		throw ShapeException("Failed to create shader program.");
	projection = ShaderUtil::recalcProjection(width, height);
}

void Shape::draw_triangle(double x1, double y1, double x2, double y2, double x3, double y3, SDL_Color color1, SDL_Color color2, SDL_Color color3, float depth) {
	if (!shaderProgram || !vbo || !vao)
		throw ShapeException("Shape not initialized.");
	glUseProgram(shaderProgram);
	//表示座標の計算
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
	glBindVertexArray(vao);
	// 色の設定
	struct normalized_color {
		float r, g, b;
	} normalized_color1 = { color1.r / 255.0f, color1.g / 255.0f, color1.b / 255.0f };
	struct normalized_color normalized_color2 = { color2.r / 255.0f, color2.g / 255.0f, color2.b / 255.0f };
	struct normalized_color normalized_color3 = { color3.r / 255.0f, color3.g / 255.0f, color3.b / 255.0f };
	// 頂点データの設定
	float vertices[3][6] = {
		{static_cast<float>(x1), static_cast<float>(y1),depth, normalized_color1.r, normalized_color1.g, normalized_color1.b},
		{static_cast<float>(x2), static_cast<float>(y2),depth, normalized_color2.r, normalized_color2.g, normalized_color2.b},
		{static_cast<float>(x3), static_cast<float>(y3),depth, normalized_color3.r, normalized_color3.g, normalized_color3.b}
	};
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	//キャッシュ作成
	int before_size = all_vertices.size();
	all_vertices.resize(before_size + 3 * 6);
	memcpy(&all_vertices.at(before_size), vertices, sizeof(vertices));
	glBufferData(GL_ARRAY_BUFFER, all_vertices.size() * sizeof(float), all_vertices.data(), GL_DYNAMIC_DRAW);
}

void Shape::draw_shapes() {
	if (all_vertices.empty()) return;
	if (!shaderProgram || !vbo || !vao)
		throw ShapeException("Shape not initialized.");
	glUseProgram(shaderProgram);
	glBindVertexArray(vao);
	glDrawArrays(GL_TRIANGLES, 0, all_vertices.size() / 6);
	glBindVertexArray(0);
}


