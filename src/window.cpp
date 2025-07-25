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
	window_mode = mode;

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
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
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
	texture_handle = glGetTextureHandleARB(texture);
	glMakeTextureHandleResidentARB(texture_handle);

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
	view = glm::mat4(1.0f); // 初期ビュー行列は単位行列

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
	if (is3D) {
		projection = glm::perspective(glm::radians(45.0f), static_cast<float>(width) / static_cast<float>(height), 0.1f, 100.0f);
	} else {
		projection = ShaderUtil::recalcProjection(width, height);
	}
	projectionID++;
	SDL_SetWindowSize(window, width, height);

	// 古いテクスチャハンドルを非居住化
	glMakeTextureHandleNonResidentARB(texture_handle);
	glDeleteTextures(1, &texture);

	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	// 新しいテクスチャの生成
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// レンダーバッファの再設定
	texture_handle = glGetTextureHandleARB(texture);
	glMakeTextureHandleResidentARB(texture_handle);

	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0.0, width, height, 0.0, -1.0, 1.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void Window::SetTitle(const std::string& title) const {
	if (SDL_SetWindowTitle(window, title.c_str()) < 0)
		throw WindowException("Failed to set window title: " + string(SDL_GetError()));
}

void Window::SetCameraPos(float x, float y, float z, float target_x, float target_y, float target_z) {
	glm::vec3 UPvec(0.0f, 0.0f, 1.0f); // 上方向はZ軸
	CameraPos = glm::vec3(x, y, z);
	const glm::vec3 ViewVec = glm::normalize(glm::vec3(target_x - x, target_y - y, target_z - z));
	if (abs(glm::dot(ViewVec, UPvec)) > 0.999f)
		UPvec = glm::vec3(0.0f, 1.0f, 0.0f); // Z軸と平行な場合はY軸を上方向に設定
	view = glm::lookAt(CameraPos, glm::vec3(target_x, target_y, target_z), UPvec);
	projection = glm::infinitePerspective(glm::radians(45.0f), static_cast<float>(width) / static_cast<float>(height), 0.1f);
	is3D = true; // 3Dモードに設定
	projectionID++;
	glEnable(GL_DEPTH_TEST);
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

void Window::Clear(SDL_Color sys_col) {
	all_vertices.clear(); // 全ての頂点データをクリア
	all_3D_vertices.clear(); // 3Dモードの頂点データをクリア
	pos = { 0, 0, 0 }; // 表示位置を初期化
	color = { 0, 0, 0, 255 };
	projectionID = 0;
	system_color = move(sys_col); // システム色を設定
	fill(colors.begin(), colors.end(), color); // 色のリストを初期化
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	shape.SetTexture(0, false);
}

void Window::Reset3D() {
	view = glm::mat4(1.0f);
	projection = ShaderUtil::recalcProjection(width, height);
	is3D = false; // 3Dモードを解除
	projectionID++;
	glDisable(GL_DEPTH_TEST);
}

void Window::DrawToDefault() {
	glBindFramebuffer(GL_FRAMEBUFFER, 0); // デフォルトのフレームバッファに戻す
	glViewport(0, 0, width, height); // ビューポートをウィンドウサイズに設定
	glClearColor(system_color.r / 255.0f, system_color.g / 255.0f, system_color.b / 255.0f, system_color.a / 255.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // カラーバッファと深度バッファをクリア

	glUseProgram(shaderProgram);

	GLint location = glGetUniformLocation(shaderProgram, "image");
	glUniformHandleui64ARB(location, texture_handle);

	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * 6, vertex, GL_STATIC_DRAW);
	glDrawArrays(GL_TRIANGLES, 0, 6); // スクリーンクワッドを描画
	glBindVertexArray(0);
}