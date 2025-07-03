#include <GL/glew.h>
#define STB_IMAGE_IMPLEMENTATION
#include "Image.hpp"
using namespace std;

void Image::Init(int width, int height) {
	this->width = width;
	this->height = height;

	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	// 頂点バッファを初期化(posX,posY,texX,texY,color)
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 8, nullptr, GL_DYNAMIC_DRAW);
	// 位置属性 (vec3)
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// テクスチャ座標属性 (vec2)
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	// 色属性 (vec3)
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5 * sizeof(float)));
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	shaderProgram = ShaderUtil::createShaderProgram(vertexShaderSource, fragmentShaderSource);
	if (!shaderProgram)
		throw ImageException("Failed to create image shader program.");
	projection = ShaderUtil::recalcProjection(width, height);
}

void Image::Load(const std::string& file_path, int id, int center_x, int center_y) {
	int width, height, channels;
	unsigned char* data = stbi_load(file_path.c_str(), &width, &height, &channels, 0);
	if (!data)
		throw ImageException("Failed to load image: " + file_path);
	images[id] = move(image_data{ width, height, channels,center_x,center_y,0 });

	glGenTextures(1, &images[id].texture);
	glBindTexture(GL_TEXTURE_2D, images[id].texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, images[id].width, images[id].height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);
	stbi_image_free(data);
}

void Image::DrawImage(unsigned int id, float x, float y, float x_size, float y_size, float angle, SDL_Color color1, SDL_Color color2, SDL_Color color3, SDL_Color color4, float depth) {
	if (!images.count(id))
		throw ImageException("Image ID not found: " + to_string(id));
	if (!shaderProgram || !vbo || !vao)
		throw ImageException("Image not initialized.");
	glUseProgram(shaderProgram);
	// 表示座標の計算
	glm::mat4 transform = glm::mat4(1.0f);
	transform = glm::translate(transform, glm::vec3(x, y, 0.0f));
	transform = glm::rotate(transform, angle, glm::vec3(0.0f, 0.0f, 1.0f));
	transform = glm::scale(transform, glm::vec3(x_size, y_size, 1.0f));

	glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "transform"), 1, GL_FALSE, glm::value_ptr(transform));
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
	glBindVertexArray(vao);
	// 色の設定
	struct normalized_color {
		float r, g, b;
	} normalized_color1 = { color1.r / 255.0f, color1.g / 255.0f, color1.b / 255.0f };
	struct normalized_color normalized_color2 = { color2.r / 255.0f, color2.g / 255.0f, color2.b / 255.0f };
	struct normalized_color normalized_color3 = { color3.r / 255.0f, color3.g / 255.0f, color3.b / 255.0f };
	struct normalized_color normalized_color4 = { color4.r / 255.0f, color4.g / 255.0f, color4.b / 255.0f };
	//位置
	float min_x = -images[id].center_x;
	float min_y = -images[id].center_y;
	float max_x = images[id].width - images[id].center_x;
	float max_y = images[id].height - images[id].center_y;
	// 頂点データの設定
	float vertices[6][8] = {
		// 位置 (x, y, depth), テクスチャ座標 (texX, texY), 色 (r, g, b)
		{min_x , min_y , depth, 0.0f, 0.0f, normalized_color1.r, normalized_color1.g, normalized_color1.b},
		{max_x , min_y , depth, 1.0f, 0.0f, normalized_color2.r, normalized_color2.g, normalized_color2.b},
		{min_x , max_y , depth, 0.0f, 1.0f, normalized_color4.r, normalized_color4.g, normalized_color4.b},

		{max_x , min_y , depth, 1.0f, 0.0f, normalized_color2.r, normalized_color2.g, normalized_color2.b},
		{max_x , max_y , depth, 1.0f, 1.0f, normalized_color3.r, normalized_color3.g, normalized_color3.b},
		{min_x , max_y , depth, 0.0f, 1.0f, normalized_color4.r, normalized_color4.g, normalized_color4.b}
	};
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	int before_size = all_image_vertices.size();
	all_image_vertices.resize(before_size + 6 * 11);
	memcpy(&all_image_vertices.at(before_size), vertices, sizeof(vertices));
	glBufferData(GL_ARRAY_BUFFER, all_image_vertices.size() * sizeof(float), all_image_vertices.data(), GL_DYNAMIC_DRAW);
}

void Image::Draw() {
	if (all_image_vertices.empty()) return;
	if (!shaderProgram || !vbo || !vao)
		throw ImageException("Image not initialized.");
	glUseProgram(shaderProgram);
	glBindVertexArray(vao);
	glDrawArrays(GL_TRIANGLES, 0, all_image_vertices.size() / 9);
	glBindVertexArray(0);
}