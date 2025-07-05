#include <GL/glew.h>
#include "shape.hpp"
#define M_PI 3.14159265358979323846f
using namespace std;

Shape::~Shape() {
	// OpenGLのリソースを解放
	if (vao) glDeleteVertexArrays(1, &vao);
	if (vbo) glDeleteBuffers(1, &vbo);
	if (shaderProgram_triangle) glDeleteProgram(shaderProgram_triangle);
	if (vao_roundrect) glDeleteVertexArrays(1, &vao_roundrect);
	if (vbo_roundrect) glDeleteBuffers(1, &vbo_roundrect);
	if (shaderProgram_roundrect) glDeleteProgram(shaderProgram_roundrect);
}

void Shape::Init(int w, int h) {
	width = w;
	height = h;
	// 三角形の頂点配列オブジェクトとバッファオブジェクトを生成
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	// 頂点バッファを初期化(posX,posY,color)
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * 7, nullptr, GL_DYNAMIC_DRAW);
	// 位置属性 (vec3)
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// 色属性 (vec4)
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	// 丸角四角の頂点配列オブジェクトとバッファオブジェクトを生成
	glGenVertexArrays(1, &vao_roundrect);
	glGenBuffers(1, &vbo_roundrect);
	glBindVertexArray(vao_roundrect);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_roundrect);
	// 頂点バッファを初期化(posX,posY,radius,color,posX,posY,box_wodth,box_height)
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 12, nullptr, GL_DYNAMIC_DRAW);
	// 位置属性 (vec3)
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 12 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// 角の半径属性 (float)
	glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, 12 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	// 色属性 (vec4)
	glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 12 * sizeof(float), (void*)(4 * sizeof(float)));
	glEnableVertexAttribArray(2);
	// 四角形の位置とサイズ属性 (vec4)
	glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 12 * sizeof(float), (void*)(8 * sizeof(float)));
	glEnableVertexAttribArray(3);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	// 線分の頂点配列オブジェクトとバッファオブジェクトを生成
	glGenVertexArrays(1, &vao_line);
	glGenBuffers(1, &vbo_line);
	glBindVertexArray(vao_line);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_line);
	// 頂点バッファを初期化(posX,posY,color)
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 2 * 7, nullptr, GL_DYNAMIC_DRAW);
	// 位置属性 (vec3)
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// 色属性 (vec3)
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	// (楕)円の頂点配列オブジェクトとバッファオブジェクトを生成
	glGenVertexArrays(1, &vao_ellipse);
	glGenBuffers(1, &vbo_ellipse);
	glBindVertexArray(vao_ellipse);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_ellipse);
	// 頂点バッファを初期化(posX,posY,depth,centerX,centerY,major_axis,minor_axis,angle,color)
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 12, nullptr, GL_DYNAMIC_DRAW);
	// 位置属性 (vec3)
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 12 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// 中心位置属性 (vec2)
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 12 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	// 長軸属性 (float)
	glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 12 * sizeof(float), (void*)(5 * sizeof(float)));
	glEnableVertexAttribArray(2);
	// 短軸属性 (float)
	glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, 12 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(3);
	// 角度属性 (float)
	glVertexAttribPointer(4, 1, GL_FLOAT, GL_FALSE, 12 * sizeof(float), (void*)(7 * sizeof(float)));
	glEnableVertexAttribArray(4);
	// 色属性 (vec4)
	glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, 12 * sizeof(float), (void*)(8 * sizeof(float)));
	glEnableVertexAttribArray(5);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	// シェーダープログラムの作成
	shaderProgram_triangle = ShaderUtil::createShaderProgram(vertexShaderSource, fragmentShaderSource);
	if (!shaderProgram_triangle)
		throw ShapeException("Failed to create triangle shader program.");
	shaderProgram_roundrect = ShaderUtil::createShaderProgram(vertexShaderSource_roundrect, fragmentShaderSource_roundrect);
	if (!shaderProgram_roundrect)
		throw ShapeException("Failed to create round rectangle shader program.");
	shaderProgram_ellipse = ShaderUtil::createShaderProgram(vertexShaderSource_ellipse, fragmentShaderSource_ellipse);
	if (!shaderProgram_ellipse)
		throw ShapeException("Failed to create ellipse shader program.");
	projection = ShaderUtil::recalcProjection(width, height);
}

void Shape::draw_triangle(float x1, float y1, float x2, float y2, float x3, float y3, SDL_Color color1, SDL_Color color2, SDL_Color color3, float depth) {
	if (!shaderProgram_triangle || !vbo || !vao)
		throw ShapeException("Shape not initialized.");
	glUseProgram(shaderProgram_triangle);
	//表示座標の計算
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram_triangle, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
	glBindVertexArray(vao);
	// 色の設定
	struct normalized_color {
		float r, g, b, a;
	} normalized_color1 = { color1.r / 255.0f, color1.g / 255.0f, color1.b / 255.0f, color1.a / 255.0f };
	struct normalized_color normalized_color2 = { color2.r / 255.0f, color2.g / 255.0f, color2.b / 255.0f, color2.a / 255.0f };
	struct normalized_color normalized_color3 = { color3.r / 255.0f, color3.g / 255.0f, color3.b / 255.0f, color3.a / 255.0f };
	// 頂点データの設定
	float vertices[3][7] = {
		{x1, y1,depth, normalized_color1.r, normalized_color1.g, normalized_color1.b, normalized_color1.a},
		{x2, y2,depth, normalized_color2.r, normalized_color2.g, normalized_color2.b, normalized_color2.a},
		{x3, y3,depth, normalized_color3.r, normalized_color3.g, normalized_color3.b, normalized_color3.a}
	};
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	//キャッシュ作成
	all_triangle_vertices.insert(all_triangle_vertices.end(), &vertices[0][0], &vertices[0][0] + 3 * 7);
	glBufferData(GL_ARRAY_BUFFER, all_triangle_vertices.size() * sizeof(float), all_triangle_vertices.data(), GL_DYNAMIC_DRAW);
}

void Shape::draw_round_rectangle(float x, float y, float width, float height, float radius, SDL_Color color1, SDL_Color color2, SDL_Color color3, SDL_Color color4, float depth) {
	if (!shaderProgram_roundrect || !vbo_roundrect || !vao_roundrect)
		throw ShapeException("Shape not initialized.");
	glUseProgram(shaderProgram_roundrect);
	//表示座標の計算
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram_roundrect, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
	glBindVertexArray(vao_roundrect);
	// 色の設定
	struct normalized_color {
		float r, g, b, a;
	} normalized_color1 = { color1.r / 255.0f, color1.g / 255.0f, color1.b / 255.0f, color1.a / 255.0f };
	struct normalized_color normalized_color2 = { color2.r / 255.0f, color2.g / 255.0f, color2.b / 255.0f, color2.a / 255.0f };
	struct normalized_color normalized_color3 = { color3.r / 255.0f, color3.g / 255.0f, color3.b / 255.0f, color3.a / 255.0f };
	struct normalized_color normalized_color4 = { color4.r / 255.0f, color4.g / 255.0f, color4.b / 255.0f, color4.a / 255.0f };
	// 角の座標を指定
	float x_min = x - radius;
	float y_min = y - radius;
	float x_max = x + width + radius;
	float y_max = y + height + radius;

	// 頂点データの設定
	float vertices[6][12] = {
		// Triangle 1
		{x_min, y_min, depth, radius, normalized_color1.r, normalized_color1.g, normalized_color1.b,normalized_color1.a, x, y, width, height}, // Bottom-Left
		{x_max, y_min, depth, radius, normalized_color2.r, normalized_color2.g, normalized_color2.b,normalized_color2.a, x, y, width, height}, // Bottom-Right
		{x_min, y_max, depth, radius, normalized_color4.r, normalized_color4.g, normalized_color4.b,normalized_color4.a, x, y, width, height}, // Top-Left
		// Triangle 2
		{x_max, y_min, depth, radius, normalized_color2.r, normalized_color2.g, normalized_color2.b,normalized_color2.a, x, y, width, height}, // Bottom-Right
		{x_max, y_max, depth, radius, normalized_color3.r, normalized_color3.g, normalized_color3.b,normalized_color3.a, x, y, width, height}, // Top-Right
		{x_min, y_max, depth, radius, normalized_color4.r, normalized_color4.g, normalized_color4.b,normalized_color4.a, x, y, width, height}  // Top-Left
	};
	glBindBuffer(GL_ARRAY_BUFFER, vbo_roundrect);
	//キャッシュ作成
	all_roundrect_vertices.insert(all_roundrect_vertices.end(), &vertices[0][0], &vertices[0][0] + 6 * 12);
	glBufferData(GL_ARRAY_BUFFER, all_roundrect_vertices.size() * sizeof(float), all_roundrect_vertices.data(), GL_DYNAMIC_DRAW);
}

void Shape::draw_line(float x1, float y1, float x2, float y2, SDL_Color color1, SDL_Color color2, float depth) {
	if (!shaderProgram_triangle || !vbo_line || !vao_line)
		throw ShapeException("Shape not initialized.");
	glUseProgram(shaderProgram_triangle);
	//表示座標の計算
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram_triangle, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
	glBindVertexArray(vao_line);
	// 色の設定
	struct normalized_color {
		float r, g, b, a;
	} normalized_color1 = { color1.r / 255.0f, color1.g / 255.0f, color1.b / 255.0f, color1.a / 255.0f };
	struct normalized_color normalized_color2 = { color2.r / 255.0f, color2.g / 255.0f, color2.b / 255.0f, color2.a / 255.0f };
	// 頂点データの設定
	float vertices[2][7] = {
		{x1, y1, depth, normalized_color1.r, normalized_color1.g, normalized_color1.b, normalized_color1.a},
		{x2, y2, depth, normalized_color2.r, normalized_color2.g, normalized_color2.b, normalized_color2.a}
	};
	glBindBuffer(GL_ARRAY_BUFFER, vbo_line);
	//キャッシュ作成
	all_line_vertices.insert(all_line_vertices.end(), &vertices[0][0], &vertices[0][0] + 2 * 7);
	glBufferData(GL_ARRAY_BUFFER, all_line_vertices.size() * sizeof(float), all_line_vertices.data(), GL_DYNAMIC_DRAW);
}

void Shape::draw_ellipse(float center_x, float center_y, float major_axis, float minor_axis, float angle, SDL_Color color1, SDL_Color color2, SDL_Color color3, SDL_Color color4, float depth) {
	if (!shaderProgram_ellipse || !vbo_ellipse || !vao_ellipse)
		throw ShapeException("Shape not initialized.");
	glUseProgram(shaderProgram_ellipse);
	//表示座標の計算
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram_ellipse, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
	glBindVertexArray(vao_ellipse);
	// 色の設定
	struct normalized_color {
		float r, g, b, a;
	} normalized_color1 = { color1.r / 255.0f, color1.g / 255.0f, color1.b / 255.0f, color1.a / 255.0f };
	struct normalized_color normalized_color2 = { color2.r / 255.0f, color2.g / 255.0f, color2.b / 255.0f, color2.a / 255.0f };
	struct normalized_color normalized_color3 = { color3.r / 255.0f, color3.g / 255.0f, color3.b / 255.0f, color3.a / 255.0f };
	struct normalized_color normalized_color4 = { color4.r / 255.0f, color4.g / 255.0f, color4.b / 255.0f, color4.a / 255.0f };
	// 角の座標を指定
	float Xlim = sqrt(pow(major_axis * cos(angle), 2) + pow(minor_axis * sin(angle), 2));
	float Ylim = sqrt(pow(major_axis * sin(angle), 2) + pow(minor_axis * cos(angle), 2));

	float min_x = center_x - Xlim;
	float min_y = center_y - Ylim;
	float max_x = center_x + Xlim;
	float max_y = center_y + Ylim;
	// 頂点データの設定
	float vertices[6][12] = {
		// 中心位置と軸の長さ、角度、色を含む
		{min_x, min_y,depth, center_x, center_y, major_axis, minor_axis, angle, normalized_color1.r, normalized_color1.g, normalized_color1.b, normalized_color1.a},
		{max_x, min_y,depth, center_x, center_y, major_axis, minor_axis, angle, normalized_color2.r, normalized_color2.g, normalized_color2.b, normalized_color2.a},
		{min_x, max_y,depth, center_x, center_y, major_axis, minor_axis, angle, normalized_color4.r, normalized_color4.g, normalized_color4.b, normalized_color4.a},

		{max_x, min_y,depth, center_x, center_y, major_axis, minor_axis, angle, normalized_color2.r, normalized_color2.g, normalized_color2.b, normalized_color2.a},
		{max_x, max_y,depth, center_x, center_y, major_axis, minor_axis, angle, normalized_color3.r, normalized_color3.g, normalized_color3.b, normalized_color3.a},
		{min_x, max_y,depth, center_x, center_y, major_axis, minor_axis, angle, normalized_color4.r, normalized_color4.g, normalized_color4.b, normalized_color4.a}
	};

	glBindBuffer(GL_ARRAY_BUFFER, vbo_ellipse);
	//キャッシュ作成
	all_ellipse_vertices.insert(all_ellipse_vertices.end(), &vertices[0][0], &vertices[0][0] + 6 * 12);
	glBufferData(GL_ARRAY_BUFFER, all_ellipse_vertices.size() * sizeof(float), all_ellipse_vertices.data(), GL_DYNAMIC_DRAW);
}


void Shape::draw_shapes() {
	if (all_triangle_vertices.empty())return;
	if (!shaderProgram_triangle || !vbo || !vao)
		throw ShapeException("Shape not initialized.");
	glUseProgram(shaderProgram_triangle);
	glBindVertexArray(vao);
	glDrawArrays(GL_TRIANGLES, 0, all_triangle_vertices.size() / 7);
	glBindVertexArray(0);
}

void Shape::draw_roundrect() {
	if (all_roundrect_vertices.empty())return;
	if (!shaderProgram_roundrect || !vbo_roundrect || !vao_roundrect)
		throw ShapeException("Shape not initialized.");
	glUseProgram(shaderProgram_roundrect);
	glBindVertexArray(vao_roundrect);
	glDrawArrays(GL_TRIANGLES, 0, all_roundrect_vertices.size() / 12);
	glBindVertexArray(0);
}

void Shape::draw_lines() {
	if (all_line_vertices.empty())return;
	if (!shaderProgram_triangle || !vbo_line || !vao_line)
		throw ShapeException("Shape not initialized.");
	glUseProgram(shaderProgram_triangle);
	glBindVertexArray(vao_line);
	glDrawArrays(GL_LINES, 0, all_line_vertices.size() / 7);
	glBindVertexArray(0);
}

void Shape::draw_ellipses() {
	if (all_ellipse_vertices.empty())return;
	if (!shaderProgram_ellipse || !vbo_ellipse || !vao_ellipse)
		throw ShapeException("Shape not initialized.");
	glUseProgram(shaderProgram_ellipse);
	glBindVertexArray(vao_ellipse);
	glDrawArrays(GL_TRIANGLES, 0, all_ellipse_vertices.size() / 12);
	glBindVertexArray(0);
}

void Shape::Clear() {
	all_triangle_vertices.clear();
	all_roundrect_vertices.clear();
	all_line_vertices.clear();
	all_ellipse_vertices.clear();
}
