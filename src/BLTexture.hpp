#ifndef BL_TEXTURE_HPP
#define BL_TEXTURE_HPP
#include "exception.hpp"
#include <SDL3/SDL_opengl.h>

class BLTexture {
private:
	GLuint textureID = 0;
	GLuint64 bindlessTextureHandle = 0;
public:
	BLTexture() = default;
	~BLTexture() { destroy(); }

	BLTexture(const BLTexture&) = delete; // コピーコンストラクタは削除
	BLTexture& operator=(const BLTexture&) = delete; // コピー代入演算子は削除
	BLTexture(BLTexture&& other) noexcept; // ムーブコンストラクタ
	BLTexture& operator=(BLTexture&& other) noexcept; // ムーブ代入演算子

	std::pair<int, int> load(const char* file_path);
	void create(size_t width, size_t height, GLenum type = GL_RGBA8, GLenum type2 = GL_RGBA);
	void update_texture(unsigned int x, unsigned int y, unsigned int width, unsigned int height, const void* data, GLenum type = GL_RGBA);
	void destroy();

	GLuint getTextureID() const { return textureID; }
	GLuint64 getBindlessTextureHandle() const { return bindlessTextureHandle; };
	bool isValid() const { return bindlessTextureHandle != 0; };
};

#endif