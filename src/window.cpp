#include "window.hpp"
using namespace std;

Window::~Window() {
	if (window) SDL_DestroyWindow(window);
	if (glContext) SDL_GL_DestroyContext(glContext);
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
	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0.0, width, height, 0.0, -1.0, 1.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_TEXTURE_2D);

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
	SDL_SetWindowSize(window, width, height);
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
}

void Window::Destroy() const {
	if (window)
		SDL_DestroyWindow(window);
	if (glContext)
		SDL_GL_DestroyContext(glContext);
}