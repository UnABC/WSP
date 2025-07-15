#include <GL/glew.h>
#include "Image.hpp"
using namespace std;

void Image::Init(map<unsigned int, image_data>& global_images, glm::mat4* proj, glm::mat4* global_view) {
	projection = proj;
	view = global_view;

	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	// 頂点バッファを初期化(posX,posY,texX,texY,color)
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 11, nullptr, GL_DYNAMIC_DRAW);
	// 位置属性 (vec3)
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// テクスチャ座標属性 (vec2)
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	// 色属性 (vec4)
	glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(5 * sizeof(float)));
	glEnableVertexAttribArray(2);
	// テクスチャハンドル属性 (uvec2)
	glVertexAttribIPointer(3, 2, GL_UNSIGNED_INT, 11 * sizeof(float), (void*)(9 * sizeof(float)));
	glEnableVertexAttribArray(3);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	shaderProgram = ShaderUtil::createShaderProgram(vertexShaderSource, fragmentShaderSource);
	if (!shaderProgram)
		throw ImageException("Failed to create image shader program.");
	images = &global_images;
}

void Image::Load(const std::string& file_path, int id, int center_x, int center_y) {
	pair<int, int> size = (*images)[id].texture.load(file_path.c_str());
	(*images)[id].width = size.first;
	(*images)[id].height = size.second;
	(*images)[id].center_x = center_x;
	(*images)[id].center_y = center_y;
}

void Image::DrawImage(unsigned int id, float x, float y, float x_size, float y_size, float angle, int tex_x, int tex_y, int tex_width, int tex_height, SDL_Color color1, SDL_Color color2, SDL_Color color3, SDL_Color color4, int gmode, vector<AllVertexData>& all_vertices) {
	if (!images->count(id))
		throw ImageException("Image ID not found: " + to_string(id));
	if (!shaderProgram || !vbo || !vao)
		throw ImageException("Image not initialized.");

	// 表示座標の計算
	glm::mat4 transform = glm::mat4(1.0f);
	transform = glm::translate(transform, glm::vec3(x, y, 0.0f));
	transform = glm::rotate(transform, angle, glm::vec3(0.0f, 0.0f, 1.0f));
	transform = glm::scale(transform, glm::vec3(x_size, y_size, 1.0f));

	GLuint64 handle = (*images)[id].texture.getBindlessTextureHandle();
	GLuint handle1 = static_cast<GLuint>(handle & 0xFFFFFFFF);
	GLuint handle2 = static_cast<GLuint>(handle >> 32);

	// 色の設定
	struct normalized_color {
		float r, g, b, a;
	} normalized_color1 = { color1.r / 255.0f, color1.g / 255.0f, color1.b / 255.0f ,color1.a / 255.0f };
	struct normalized_color normalized_color2 = { color2.r / 255.0f, color2.g / 255.0f, color2.b / 255.0f, color2.a / 255.0f };
	struct normalized_color normalized_color3 = { color3.r / 255.0f, color3.g / 255.0f, color3.b / 255.0f, color3.a / 255.0f };
	struct normalized_color normalized_color4 = { color4.r / 255.0f, color4.g / 255.0f, color4.b / 255.0f, color4.a / 255.0f };

	// テクスチャ座標の計算
	float tex_min_x = (tex_x < 0) ? 0.0f : static_cast<float>(tex_x) / (*images)[id].width;
	float tex_min_y = (tex_y < 0) ? 0.0f : static_cast<float>(tex_y) / (*images)[id].height;
	float tex_max_x = (tex_x < 0) ? 1.0f : static_cast<float>(tex_x + tex_width) / (*images)[id].width;
	float tex_max_y = (tex_y < 0) ? 1.0f : static_cast<float>(tex_y + tex_height) / (*images)[id].height;
	float tex_width_size = tex_max_x - tex_min_x;
	float tex_height_size = tex_max_y - tex_min_y;
	//位置
	float min_x = -(*images)[id].center_x;
	float min_y = -(*images)[id].center_y;
	float max_x = tex_width_size * (*images)[id].width - (*images)[id].center_x;
	float max_y = tex_height_size * (*images)[id].height - (*images)[id].center_y;

	glm::vec3 v1 = transform * glm::vec4(min_x, min_y, 0.0, 1.0f);
	glm::vec3 v2 = transform * glm::vec4(max_x, min_y, 0.0, 1.0f);
	glm::vec3 v3 = transform * glm::vec4(max_x, max_y, 0.0, 1.0f);
	glm::vec3 v4 = transform * glm::vec4(min_x, max_y, 0.0, 1.0f);

	// 頂点データの設定
	float vertices[6][11] = {
		// 位置 (x, y, 0.0), テクスチャ座標 (texX, texY), 色 (r, g, b)
		{v1.x , v1.y , v1.z, tex_min_x, tex_min_y, normalized_color1.r, normalized_color1.g, normalized_color1.b,normalized_color1.a, *(float*)&handle1, *(float*)&handle2},
		{v2.x , v2.y , v2.z, tex_max_x, tex_min_y, normalized_color2.r, normalized_color2.g, normalized_color2.b,normalized_color2.a, *(float*)&handle1, *(float*)&handle2},
		{v4.x , v4.y , v4.z, tex_min_x, tex_max_y, normalized_color4.r, normalized_color4.g, normalized_color4.b,normalized_color4.a, *(float*)&handle1, *(float*)&handle2},

		{v2.x , v2.y , v2.z, tex_max_x, tex_min_y, normalized_color2.r, normalized_color2.g, normalized_color2.b,normalized_color2.a, *(float*)&handle1, *(float*)&handle2},
		{v3.x , v3.y , v3.z, tex_max_x, tex_max_y, normalized_color3.r, normalized_color3.g, normalized_color3.b,normalized_color3.a, *(float*)&handle1, *(float*)&handle2},
		{v4.x , v4.y , v4.z, tex_min_x, tex_max_y, normalized_color4.r, normalized_color4.g, normalized_color4.b,normalized_color4.a, *(float*)&handle1, *(float*)&handle2}
	};
	int local_gmode = gmode - (gmode % 2);
	if (all_vertices.empty() || (all_vertices.back().gmode != local_gmode) || (all_vertices.back().ID != 1)) {
		AllVertexData new_data;
		new_data.all_vertices = std::vector<float>();
		new_data.gmode = local_gmode;
		new_data.ID = 1;
		new_data.division = 11;
		new_data.projection = *projection;
		new_data.view = *view;
		new_data.vao = vao;
		new_data.vbo = vbo;
		new_data.shaderProgram = shaderProgram;
		all_vertices.push_back(new_data);
	}
	all_vertices.back().all_vertices.insert(all_vertices.back().all_vertices.end(), &vertices[0][0], &vertices[0][0] + 6 * 11);
}