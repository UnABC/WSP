#include <GL/glew.h>
#include "BLTexture.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <utility>
using namespace std;

BLTexture::BLTexture(BLTexture&& other) noexcept
	: textureID(exchange(other.textureID, 0)), bindlessTextureHandle(exchange(other.bindlessTextureHandle, 0)) {
}

BLTexture& BLTexture::operator=(BLTexture&& other) noexcept {
	if (this != &other) {
		destroy();
		textureID = exchange(other.textureID, 0);
		bindlessTextureHandle = exchange(other.bindlessTextureHandle, 0);
	}
	return *this;
}

pair<int, int> BLTexture::load(const char* file_path) {
	if (textureID != 0) destroy(); // 既存のテクスチャを破棄

	int width, height, channels;
	unsigned char* data = stbi_load(file_path, &width, &height, &channels, 0);
	if (!data) throw ImageException("Failed to load texture: " + string(file_path));

	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);
	stbi_image_free(data);

	bindlessTextureHandle = glGetTextureHandleARB(textureID);
	glMakeTextureHandleResidentARB(bindlessTextureHandle);
	glBindTexture(GL_TEXTURE_2D, 0);

	return make_pair(width, height);
}

void BLTexture::create(size_t width, size_t height, GLenum type, GLenum type2) {
	if (textureID != 0) destroy(); // 既存のテクスチャを破棄

	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, type, static_cast<GLsizei>(width), static_cast<GLsizei>(height), 0, type2, GL_UNSIGNED_BYTE, nullptr);

	bindlessTextureHandle = glGetTextureHandleARB(textureID);
	glMakeTextureHandleResidentARB(bindlessTextureHandle);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void BLTexture::update_texture(unsigned int x, unsigned int y, unsigned int width, unsigned int height, const void* data, GLenum type) {
	if (textureID == 0) throw ImageException("Texture not created.");
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexSubImage2D(GL_TEXTURE_2D, 0, x, y, width, height, type, GL_UNSIGNED_BYTE, data);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void BLTexture::destroy() {
	if (bindlessTextureHandle != 0) {
		glMakeTextureHandleNonResidentARB(bindlessTextureHandle);
		bindlessTextureHandle = 0;
	}
	if (textureID != 0) {
		glDeleteTextures(1, &textureID);
		textureID = 0;
	}
}
