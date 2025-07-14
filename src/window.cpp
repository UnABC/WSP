#include <GL/glew.h>
#include "window.hpp"
using namespace std;

Window::~Window() {
	Destroy();
}

void Window::Create(bool isfirst, const std::string& title, int width, int height, int mode) {
	this->width = width;
	this->height = height;
	this->is_fullscreen = mode & 1;

	Uint32 flags = SDL_WINDOW_OPENGL;
	if (mode & 1)
		flags |= SDL_WINDOW_FULLSCREEN;
	if (mode & 2)
		flags |= SDL_WINDOW_BORDERLESS;
	if (mode & 4)
		flags |= SDL_WINDOW_MAXIMIZED;
	if (mode & 8)
		flags |= SDL_WINDOW_MINIMIZED;
	if (mode & 16)
		flags |= SDL_WINDOW_HIDDEN;
	if (mode & 32)
		flags |= SDL_WINDOW_ALWAYS_ON_TOP;
	if (mode & 64)
		flags |= SDL_WINDOW_RESIZABLE;

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 5);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

	window = SDL_CreateWindow(title.c_str(), width, height, flags);
	if (!window)
		throw WindowException("Failed to create SDL Window: " + string(SDL_GetError()));

	SDL_GL_SetAttribute(SDL_GL_SHARE_WITH_CURRENT_CONTEXT, isfirst ? 0 : 1);
	glContext = SDL_GL_CreateContext(window);
	if (!glContext)
		throw WindowException("Failed to create OpenGL context: " + string(SDL_GetError()));
	if (SDL_GL_MakeCurrent(window, glContext) < 0)
		throw WindowException("Failed to make OpenGL context current: " + string(SDL_GetError()));
	// GLEWの初期化
	glewExperimental = GL_TRUE;
	GLenum err = glewInit();
	if (err != GLEW_OK)
		throw WindowException(reinterpret_cast<const char*>(glewGetErrorString(err)));
	if (isfirst && !GLEW_ARB_bindless_texture)
		throw WindowException("OpenGL does not support bindless textures.");

	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0.0, width, height, 0.0, -1.0, 1.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_TEXTURE_2D);

	// フレームバッファオブジェクトの生成
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	// テクスチャの生成
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// レンダーバッファオブジェクトの生成
	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		throw WindowException("Framebuffer is not complete: " + string(SDL_GetError()));
	// フレームバッファのバインドを解除
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	// ビューポートの設定
	glViewport(0, 0, width, height);

	//スクリーンクワッド用
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	// 頂点バッファを初期化(posX,posY)
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * 6, nullptr, GL_DYNAMIC_DRAW);
	// 位置属性 (vec3)
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	// シェーダープログラムの作成
	shaderProgram = ShaderUtil::createShaderProgram(vertexShaderSource, fragmentShaderSource);
	if (!shaderProgram)
		throw WindowException("Failed to create window shader program: " + string(SDL_GetError()));
	// プロジェクション行列の初期化
	projection = ShaderUtil::recalcProjection(width, height);

	//画面の初期化
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	if (!SDL_GL_SwapWindow(window))
		throw WindowException("Failed to swap OpenGL window: " + string(SDL_GetError()));

}

void Window::SetFullscreen(bool fullscreen) {
	if (fullscreen == is_fullscreen) return; // 既に同じ状態なら何もしない
	is_fullscreen = fullscreen;
	Uint32 flags = SDL_GetWindowFlags(window);
	if (fullscreen)
		flags |= SDL_WINDOW_FULLSCREEN;
	else
		flags &= ~SDL_WINDOW_FULLSCREEN;
	SDL_SetWindowFullscreen(window, flags);
}

void Window::Resize(int new_width, int new_height) {
	if (new_width <= 0 || new_height <= 0) return; // 無効なサイズは無視
	width = new_width;
	height = new_height;
	projection = ShaderUtil::recalcProjection(new_width, new_height);
	SDL_SetWindowSize(window, width, height);

	// FBOのテクスチャとレンダーバッファをリサイズ
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
	glBindTexture(GL_TEXTURE_2D, 0);

	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0.0, new_width, new_height, 0.0, -1.0, 1.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void Window::SetTitle(const std::string& title) const {
	if (SDL_SetWindowTitle(window, title.c_str()) < 0) {
		throw WindowException("Failed to set window title: " + string(SDL_GetError()));
	}
}

void Window::SetPosition(int x, int y) {
	SDL_SetWindowPosition(window, x, y);
}

void Window::MakeCurrent() {
	if (SDL_GL_MakeCurrent(window, glContext) < 0)
		throw WindowException("Failed to make OpenGL context current: " + string(SDL_GetError()));
	glViewport(0, 0, width, height);
}

void Window::Destroy() const {
	if (window)
		SDL_DestroyWindow(window);
	if (glContext)
		SDL_GL_DestroyContext(glContext);
}

void Window::DrawToDefault() {
	glBindFramebuffer(GL_FRAMEBUFFER, 0); // デフォルトのフレームバッファに戻す
	glViewport(0, 0, width, height); // ビューポートをウィンドウサイズに設定
	glClearColor(system_color.r / 255.0f, system_color.g / 255.0f, system_color.b / 255.0f, system_color.a / 255.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // カラーバッファと深度バッファをクリア

	glUseProgram(shaderProgram);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	glUniform1i(glGetUniformLocation(shaderProgram, "image"), 0);

	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * 6, vertex, GL_STATIC_DRAW);
	glDrawArrays(GL_TRIANGLES, 0, 6); // スクリーンクワッドを描画
	glBindVertexArray(0);
}