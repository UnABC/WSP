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

void Shape::Init(int w, int h, glm::mat4* proj, glm::mat4* global_view, int* projID, vector<AllVertexData>* all_vertices_ptr) {
	width = w;
	height = h;
	projection = proj;
	view = global_view;
	projectionID = projID;
	all_vertices = all_vertices_ptr;
	// 三角形の頂点配列オブジェクトとバッファオブジェクトを生成
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	// 頂点バッファを初期化(posX,posY,color)
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * 12, nullptr, GL_DYNAMIC_DRAW);
	// 位置属性 (vec3)
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 12 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// 色属性 (vec4)
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 12 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	// テクスチャ有効化 (bool)
	glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 12 * sizeof(float), (void*)(7 * sizeof(float)));
	glEnableVertexAttribArray(2);
	// テクスチャ座標属性 (vec2)
	glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 12 * sizeof(float), (void*)(8 * sizeof(float)));
	glEnableVertexAttribArray(3);
	// テクスチャハンドル属性 (uvec2)
	glVertexAttribIPointer(4, 2, GL_UNSIGNED_INT, 12 * sizeof(float), (void*)(10 * sizeof(float)));
	glEnableVertexAttribArray(4);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	// 丸角四角の頂点配列オブジェクトとバッファオブジェクトを生成
	glGenVertexArrays(1, &vao_roundrect);
	glGenBuffers(1, &vbo_roundrect);
	glBindVertexArray(vao_roundrect);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_roundrect);
	// 頂点バッファを初期化(posX,posY,radius,color,posX,posY,box_wodth,box_height)
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 17, nullptr, GL_DYNAMIC_DRAW);
	// 位置属性 (vec3)
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 17 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// 角の半径属性 (float)
	glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, 17 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	// 色属性 (vec4)
	glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 17 * sizeof(float), (void*)(4 * sizeof(float)));
	glEnableVertexAttribArray(2);
	// 四角形の位置とサイズ属性 (vec4)
	glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 17 * sizeof(float), (void*)(8 * sizeof(float)));
	glEnableVertexAttribArray(3);
	// テクスチャ有効化 (float)
	glVertexAttribPointer(4, 1, GL_FLOAT, GL_FALSE, 17 * sizeof(float), (void*)(12 * sizeof(float)));
	glEnableVertexAttribArray(4);
	// テクスチャ座標属性 (vec2)
	glVertexAttribPointer(5, 2, GL_FLOAT, GL_FALSE, 17 * sizeof(float), (void*)(13 * sizeof(float)));
	glEnableVertexAttribArray(5);
	// テクスチャハンドル属性 (uvec2)
	glVertexAttribIPointer(6, 2, GL_UNSIGNED_INT, 17 * sizeof(float), (void*)(15 * sizeof(float)));
	glEnableVertexAttribArray(6);
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
	// 頂点バッファを初期化(posX,posY,0.0,centerX,centerY,major_axis,minor_axis,angle,color)
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 17, nullptr, GL_DYNAMIC_DRAW);
	// 位置属性 (vec3)
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 17 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// 中心位置属性 (vec2)
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 17 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	// 長軸属性 (float)
	glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 17 * sizeof(float), (void*)(5 * sizeof(float)));
	glEnableVertexAttribArray(2);
	// 短軸属性 (float)
	glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, 17 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(3);
	// 角度属性 (float)
	glVertexAttribPointer(4, 1, GL_FLOAT, GL_FALSE, 17 * sizeof(float), (void*)(7 * sizeof(float)));
	glEnableVertexAttribArray(4);
	// 色属性 (vec4)
	glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, 17 * sizeof(float), (void*)(8 * sizeof(float)));
	glEnableVertexAttribArray(5);
	// テクスチャ有効化 (float)
	glVertexAttribPointer(6, 1, GL_FLOAT, GL_FALSE, 17 * sizeof(float), (void*)(12 * sizeof(float)));
	glEnableVertexAttribArray(6);
	// テクスチャ座標属性 (vec2)
	glVertexAttribPointer(7, 2, GL_FLOAT, GL_FALSE, 17 * sizeof(float), (void*)(13 * sizeof(float)));
	glEnableVertexAttribArray(7);
	// テクスチャハンドル属性 (uvec2)
	glVertexAttribIPointer(8, 2, GL_UNSIGNED_INT, 17 * sizeof(float), (void*)(15 * sizeof(float)));
	glEnableVertexAttribArray(8);
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
}

void Shape::SetTexture(image_data* image, bool enable, float tex_x, float tex_y, float tex_width, float tex_height) {
	if (image == nullptr) {
		texture.enable = false;
		return;
	}
	texture.enable = enable;
	texture.handle = image->texture.getBindlessTextureHandle();
	texture.tex_x = tex_x / image->width;
	texture.tex_y = tex_y / image->height;
	if (tex_width <= 0.0f || tex_height <= 0.0f) {
		// テクスチャの幅と高さが0以下の場合、デフォルト値を使用
		texture.tex_width = 1.0f;
		texture.tex_height = 1.0f;
	} else {
		texture.tex_width = tex_width / image->width;
		texture.tex_height = tex_height / image->height;
	}
}

void Shape::draw_triangle(float x1, float y1, float x2, float y2, float x3, float y3, SDL_Color color1, SDL_Color color2, SDL_Color color3, int gmode, int isRect, float z1, float z2, float z3) {
	if (!shaderProgram_triangle || !vbo || !vao)
		throw ShapeException("Shape not initialized.");
	// 色の設定
	struct normalized_color {
		float r, g, b, a;
	} normalized_color1 = { color1.r / 255.0f, color1.g / 255.0f, color1.b / 255.0f, color1.a / 255.0f };
	struct normalized_color normalized_color2 = { color2.r / 255.0f, color2.g / 255.0f, color2.b / 255.0f, color2.a / 255.0f };
	struct normalized_color normalized_color3 = { color3.r / 255.0f, color3.g / 255.0f, color3.b / 255.0f, color3.a / 255.0f };
	// テクスチャの設定
	glm::fvec2 tex1, tex2, tex3;
	GLuint64 tex_handle;
	GLuint handle1, handle2;
	if (texture.enable) {
		tex1 = tex2 = tex3 = { texture.tex_x, texture.tex_y };
		//三角形
		if (isRect == 0) {
			float bounding_box_x = min(x1, min(x2, x3));
			float bounding_box_y = min(y1, min(y2, y3));
			if (texture.tex_width == 1.0f && texture.tex_height == 1.0f) {
				// テクスチャが全体を覆う場合
				tex1 += glm::fvec2{ (x1 - bounding_box_x) / width, (y1 - bounding_box_y) / height };
				tex2 += glm::fvec2{ (x2 - bounding_box_x) / width, (y2 - bounding_box_y) / height };
				tex3 += glm::fvec2{ (x3 - bounding_box_x) / width, (y3 - bounding_box_y) / height };
			} else {
				float bounding_box_width = max(x1, max(x2, x3)) - bounding_box_x;
				float bounding_box_height = max(y1, max(y2, y3)) - bounding_box_y;

				float related_x1 = (x1 - bounding_box_x) / bounding_box_width;
				float related_y1 = (y1 - bounding_box_y) / bounding_box_height;
				float related_x2 = (x2 - bounding_box_x) / bounding_box_width;
				float related_y2 = (y2 - bounding_box_y) / bounding_box_height;
				float related_x3 = (x3 - bounding_box_x) / bounding_box_width;
				float related_y3 = (y3 - bounding_box_y) / bounding_box_height;
				// テクスチャが部分的に適用される場合
				tex1 += glm::fvec2{ texture.tex_width * related_x1, texture.tex_height * related_y1 };
				tex2 += glm::fvec2{ texture.tex_width * related_x2, texture.tex_height * related_y2 };
				tex3 += glm::fvec2{ texture.tex_width * related_x3, texture.tex_height * related_y3 };
			}
			//四角形
		} else if (isRect == 1) {
			tex2 += glm::fvec2{ texture.tex_width, 0.0f };
			tex3 += glm::fvec2{ texture.tex_width, texture.tex_height };
		} else if (isRect == 2) {
			tex2 += glm::fvec2{ texture.tex_width, texture.tex_height };
			tex3 += glm::fvec2{ 0.0, texture.tex_height };
		}

		tex_handle = texture.handle;
		handle1 = static_cast<GLuint>(tex_handle & 0xFFFFFFFF);
		handle2 = static_cast<GLuint>(tex_handle >> 32);

		if ((gmode & 1) == 0)
			normalized_color1 = normalized_color2 = normalized_color3 = { 1.0, 1.0, 1.0, 1.0 };
	} else {
		tex1 = tex2 = tex3 = { 0.0f,0.0f };
		tex_handle = 0;
		handle1 = handle2 = 0;
	}

	// 頂点データの設定
	float vertices[3][12] = {
		{x1, y1,z1, normalized_color1.r, normalized_color1.g, normalized_color1.b, normalized_color1.a, texture.enable, tex1.x, tex1.y, *(float*)&handle1, *(float*)&handle2},
		{x2, y2,z2, normalized_color2.r, normalized_color2.g, normalized_color2.b, normalized_color2.a, texture.enable, tex2.x, tex2.y, *(float*)&handle1, *(float*)&handle2},
		{x3, y3,z3, normalized_color3.r, normalized_color3.g, normalized_color3.b, normalized_color3.a, texture.enable, tex3.x, tex3.y, *(float*)&handle1, *(float*)&handle2}
	};
	//キャッシュ作成
	int local_gmode = gmode - (gmode % 2);
	if (all_vertices->empty() || (all_vertices->back().gmode != local_gmode) || (all_vertices->back().ID != 2)) {
		AllVertexData new_data;
		new_data.all_vertices = std::vector<float>();
		new_data.gmode = local_gmode;
		new_data.ID = 2; // 2: 三角形
		new_data.projection = *projection;
		new_data.view = *view;
		new_data.vao = vao;
		new_data.vbo = vbo;
		new_data.shaderProgram = shaderProgram_triangle;
		new_data.division = 12;
		new_data.projectionID = *projectionID;
		all_vertices->push_back(move(new_data));
	}
	all_vertices->back().all_vertices.insert(all_vertices->back().all_vertices.end(), &vertices[0][0], &vertices[0][0] + 3 * 12);
}

void Shape::draw_round_rectangle(float x, float y, float width, float height, float radius, SDL_Color color1, SDL_Color color2, SDL_Color color3, SDL_Color color4, int gmode) {
	if (!shaderProgram_roundrect || !vbo_roundrect || !vao_roundrect)
		throw ShapeException("Shape not initialized.");
	// 色の設定
	struct normalized_color {
		float r, g, b, a;
	} normalized_color1 = { color1.r / 255.0f, color1.g / 255.0f, color1.b / 255.0f, color1.a / 255.0f };
	struct normalized_color normalized_color2 = { color2.r / 255.0f, color2.g / 255.0f, color2.b / 255.0f, color2.a / 255.0f };
	struct normalized_color normalized_color3 = { color3.r / 255.0f, color3.g / 255.0f, color3.b / 255.0f, color3.a / 255.0f };
	struct normalized_color normalized_color4 = { color4.r / 255.0f, color4.g / 255.0f, color4.b / 255.0f, color4.a / 255.0f };

	glm::fvec2 tex1, tex2, tex3, tex4;
	GLuint handle1, handle2;
	if (texture.enable) {
		tex1 = tex2 = tex3 = tex4 = { texture.tex_x, texture.tex_y };
		tex2 += glm::fvec2{ texture.tex_width, 0.0f };
		tex3 += glm::fvec2{ texture.tex_width, texture.tex_height };
		tex4 += glm::fvec2{ 0.0f, texture.tex_height };

		handle1 = static_cast<GLuint>(texture.handle & 0xFFFFFFFF);
		handle2 = static_cast<GLuint>(texture.handle >> 32);
		if ((gmode & 1) == 0)
			normalized_color1 = normalized_color2 = normalized_color3 = normalized_color4 = { 1.0, 1.0, 1.0, 1.0 };
	} else {
		handle1 = handle2 = 0;
		tex1 = tex2 = tex3 = tex4 = { 0.0f, 0.0f };
	}

	// 角の座標を指定
	float x_min = x - radius;
	float y_min = y - radius;
	float x_max = x + width + radius;
	float y_max = y + height + radius;

	// 頂点データの設定
	float vertices[6][17] = {
		// Triangle 1
		{x_min, y_min, 0.0, radius, normalized_color1.r, normalized_color1.g, normalized_color1.b,normalized_color1.a, x, y, width, height, texture.enable, tex1.x, tex1.y, *(float*)&handle1, *(float*)&handle2}, // Bottom-Left
		{x_max, y_min, 0.0, radius, normalized_color2.r, normalized_color2.g, normalized_color2.b,normalized_color2.a, x, y, width, height, texture.enable, tex2.x, tex2.y, *(float*)&handle1, *(float*)&handle2}, // Bottom-Right
		{x_min, y_max, 0.0, radius, normalized_color4.r, normalized_color4.g, normalized_color4.b,normalized_color4.a, x, y, width, height, texture.enable, tex4.x, tex4.y, *(float*)&handle1, *(float*)&handle2}, // Top-Left
		// Triangle 2
		{x_max, y_min, 0.0, radius, normalized_color2.r, normalized_color2.g, normalized_color2.b,normalized_color2.a, x, y, width, height, texture.enable, tex2.x, tex2.y, *(float*)&handle1, *(float*)&handle2}, // Bottom-Right
		{x_max, y_max, 0.0, radius, normalized_color3.r, normalized_color3.g, normalized_color3.b,normalized_color3.a, x, y, width, height, texture.enable, tex3.x, tex3.y, *(float*)&handle1, *(float*)&handle2}, // Top-Right
		{x_min, y_max, 0.0, radius, normalized_color4.r, normalized_color4.g, normalized_color4.b,normalized_color4.a, x, y, width, height, texture.enable, tex4.x, tex4.y, *(float*)&handle1, *(float*)&handle2}  // Top-Left
	};
	//キャッシュ作成
	int local_gmode = gmode - (gmode % 2);
	if (all_vertices->empty() || (all_vertices->back().gmode != local_gmode) || (all_vertices->back().ID != 3)) {
		AllVertexData new_data;
		new_data.all_vertices = std::vector<float>();
		new_data.gmode = local_gmode;
		new_data.ID = 3; // 3: 角丸四角形
		new_data.projection = *projection;
		new_data.view = *view;
		new_data.vao = vao_roundrect;
		new_data.vbo = vbo_roundrect;
		new_data.shaderProgram = shaderProgram_roundrect;
		new_data.division = 17;
		new_data.projectionID = *projectionID;
		all_vertices->push_back(new_data);
	}
	// 既存の頂点データに新しい頂点を追加
	all_vertices->back().all_vertices.insert(all_vertices->back().all_vertices.end(), &vertices[0][0], &vertices[0][0] + 6 * 17);
}

void Shape::draw_line(float x1, float y1, float x2, float y2, SDL_Color color1, SDL_Color color2, int gmode, float z1, float z2) {
	if (!shaderProgram_triangle || !vbo_line || !vao_line)
		throw ShapeException("Shape not initialized.");
	// 色の設定
	struct normalized_color {
		float r, g, b, a;
	} normalized_color1 = { color1.r / 255.0f, color1.g / 255.0f, color1.b / 255.0f, color1.a / 255.0f };
	struct normalized_color normalized_color2 = { color2.r / 255.0f, color2.g / 255.0f, color2.b / 255.0f, color2.a / 255.0f };
	// 頂点データの設定
	float vertices[2][7] = {
		{x1, y1, z1, normalized_color1.r, normalized_color1.g, normalized_color1.b, normalized_color1.a},
		{x2, y2, z2, normalized_color2.r, normalized_color2.g, normalized_color2.b, normalized_color2.a}
	};
	//キャッシュ作成
	int local_gmode = gmode - (gmode % 2);
	if (all_vertices->empty() || (all_vertices->back().gmode != local_gmode) || (all_vertices->back().ID != 4)) {
		AllVertexData new_data;
		new_data.all_vertices = std::vector<float>();
		new_data.gmode = local_gmode;
		new_data.ID = 4; // 4: 線分
		new_data.projection = *projection;
		new_data.view = *view;
		new_data.vao = vao_line;
		new_data.vbo = vbo_line;
		new_data.shaderProgram = shaderProgram_triangle;
		new_data.division = 7;
		new_data.graphics_mode = GL_LINES; // 線分の描画モード
		new_data.projectionID = *projectionID;
		all_vertices->push_back(new_data);
	}
	// 既存の頂点データに新しい頂点を追加
	all_vertices->back().all_vertices.insert(all_vertices->back().all_vertices.end(), &vertices[0][0], &vertices[0][0] + 2 * 7);
}

void Shape::draw_ellipse(float center_x, float center_y, float major_axis, float minor_axis, float angle, SDL_Color color1, SDL_Color color2, SDL_Color color3, SDL_Color color4, int gmode) {
	if (!shaderProgram_ellipse || !vbo_ellipse || !vao_ellipse)
		throw ShapeException("Shape not initialized.");
	// 色の設定
	struct normalized_color {
		float r, g, b, a;
	} normalized_color1 = { color1.r / 255.0f, color1.g / 255.0f, color1.b / 255.0f, color1.a / 255.0f };
	struct normalized_color normalized_color2 = { color2.r / 255.0f, color2.g / 255.0f, color2.b / 255.0f, color2.a / 255.0f };
	struct normalized_color normalized_color3 = { color3.r / 255.0f, color3.g / 255.0f, color3.b / 255.0f, color3.a / 255.0f };
	struct normalized_color normalized_color4 = { color4.r / 255.0f, color4.g / 255.0f, color4.b / 255.0f, color4.a / 255.0f };
	// テクスチャの設定
	glm::fvec2 tex1, tex2, tex3, tex4;
	GLuint handle1, handle2;
	if (texture.enable) {
		tex1 = tex2 = tex3 = tex4 = { texture.tex_x, texture.tex_y };
		tex2 += glm::fvec2{ texture.tex_width, 0.0f };
		tex3 += glm::fvec2{ texture.tex_width, texture.tex_height };
		tex4 += glm::fvec2{ 0.0f, texture.tex_height };

		handle1 = static_cast<GLuint>(texture.handle & 0xFFFFFFFF);
		handle2 = static_cast<GLuint>(texture.handle >> 32);
		if ((gmode & 1) == 0)
			normalized_color1 = normalized_color2 = normalized_color3 = normalized_color4 = { 1.0, 1.0, 1.0, 1.0 };
	} else {
		handle1 = handle2 = 0;
		tex1 = tex2 = tex3 = tex4 = { 0.0f, 0.0f };
	}

	// 角の座標を指定
	float Xlim = sqrt(pow(major_axis * cos(angle), 2) + pow(minor_axis * sin(angle), 2));
	float Ylim = sqrt(pow(major_axis * sin(angle), 2) + pow(minor_axis * cos(angle), 2));

	float min_x = center_x - Xlim;
	float min_y = center_y - Ylim;
	float max_x = center_x + Xlim;
	float max_y = center_y + Ylim;
	// 頂点データの設定
	float vertices[6][17] = {
		// 中心位置と軸の長さ、角度、色を含む
		{min_x, min_y,0.0, center_x, center_y, major_axis, minor_axis, angle, normalized_color1.r, normalized_color1.g, normalized_color1.b, normalized_color1.a, texture.enable, tex1.x, tex1.y, *(float*)&handle1, *(float*)&handle2},
		{max_x, min_y,0.0, center_x, center_y, major_axis, minor_axis, angle, normalized_color2.r, normalized_color2.g, normalized_color2.b, normalized_color2.a, texture.enable, tex2.x, tex2.y, *(float*)&handle1, *(float*)&handle2},
		{min_x, max_y,0.0, center_x, center_y, major_axis, minor_axis, angle, normalized_color4.r, normalized_color4.g, normalized_color4.b, normalized_color4.a, texture.enable, tex4.x, tex4.y, *(float*)&handle1, *(float*)&handle2},

		{max_x, min_y,0.0, center_x, center_y, major_axis, minor_axis, angle, normalized_color2.r, normalized_color2.g, normalized_color2.b, normalized_color2.a, texture.enable, tex2.x, tex2.y, *(float*)&handle1, *(float*)&handle2},
		{max_x, max_y,0.0, center_x, center_y, major_axis, minor_axis, angle, normalized_color3.r, normalized_color3.g, normalized_color3.b, normalized_color3.a, texture.enable, tex3.x, tex3.y, *(float*)&handle1, *(float*)&handle2},
		{min_x, max_y,0.0, center_x, center_y, major_axis, minor_axis, angle, normalized_color4.r, normalized_color4.g, normalized_color4.b, normalized_color4.a, texture.enable, tex4.x, tex4.y, *(float*)&handle1, *(float*)&handle2}
	};
	//キャッシュ作成
	int local_gmode = gmode - (gmode % 2);
	if (all_vertices->empty() || (all_vertices->back().gmode != local_gmode) || (all_vertices->back().ID != 5)) {
		AllVertexData new_data;
		new_data.all_vertices = std::vector<float>();
		new_data.gmode = local_gmode;
		new_data.ID = 5; // 5: 楕円
		new_data.projection = *projection;
		new_data.view = *view;
		new_data.vao = vao_ellipse;
		new_data.vbo = vbo_ellipse;
		new_data.shaderProgram = shaderProgram_ellipse;
		new_data.division = 17;
		new_data.projectionID = *projectionID;
		all_vertices->push_back(new_data);
	}
	// 既存の頂点データに新しい頂点を追加
	all_vertices->back().all_vertices.insert(all_vertices->back().all_vertices.end(), &vertices[0][0], &vertices[0][0] + 6 * 17);
}
