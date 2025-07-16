#ifndef WINDOW_HPP
#define WINDOW_HPP

#include "exception.hpp"
#include "shader.hpp"
#include "shape.hpp"
#include "Image.hpp"
#include "font.hpp"
#include <SDL3/SDL.h>
#include <SDL3/SDL_opengl.h>
#include <vector>

struct Position {
	float x, y, z;
	Position(float x, float y, float z = 0.0f) : x(x), y(y), z(z) {};
};

class Window {
private:
	SDL_Window* window = nullptr;
	SDL_GLContext glContext = nullptr;
	GLuint fbo, texture, rbo;
	GLuint vao, vbo, shaderProgram;
	GLuint64 texture_handle;
	int width, height;
	bool is_fullscreen;
	bool is3D = false; // 3Dモードかどうか

	const char* vertexShaderSource = R"glsl(
		#version 450 core
		layout (location = 0) in vec3 position;
		out vec2 TexCoords;
		void main() {
			gl_Position = vec4(position, 1.0);
			TexCoords = position.xy * 0.5 + 0.5; // [-1, 1] to [0, 1]
		}
	)glsl";
	const char* fragmentShaderSource = R"glsl(
		#version 450 core
		#extension GL_ARB_bindless_texture : require
		out vec4 FragColor;
		in vec2 TexCoords;
		uniform sampler2D image;
		void main() {
			FragColor = texture(image, TexCoords);
		}
	)glsl";

	const float vertex[6][3] = {
		{-1.0f, -1.0f, 0.0f},
		{ 1.0f, -1.0f, 0.0f},
		{ 1.0f,  1.0f, 0.0f},
		{-1.0f, -1.0f, 0.0f},
		{ 1.0f,  1.0f, 0.0f},
		{-1.0f,  1.0f, 0.0f}
	};
public:
	Position pos = { 0, 0, 0 };	//表示位置
	SDL_Color color = { 0, 0, 0, 255 };	//黒色
	SDL_Color system_color = { 255, 255, 255, 255 };	//システム色（白色）
	std::vector<SDL_Color> colors;	//色のリスト
	//キャッシュ
	std::vector<AllVertexData> all_vertices;	//全ての頂点データを保持するベクター

	Shape shape;
	Image image;
	Font font;
	glm::mat4 projection;
	glm::mat4 view;

	Window() : colors(4, { 0, 0, 0, 255 }) {} // 初期化時に白色を設定
	~Window();

	void Create(bool isfirst, const std::string& title, int width, int height, int mode = 0);
	void SetFullscreen(bool fullscreen);
	void Resize(int new_width, int new_height);
	void SetTitle(const std::string& title) const;
	void SetPosition(int x, int y) { SDL_SetWindowPosition(window, x, y); };
	void SetCameraPos(float x = 0.0f, float y = 0.0f, float z = 0.0f, float target_x = 0.0f, float target_y = 0.0f, float target_z = 0.0f);
	void MakeCurrent();
	void MakeTop() { SDL_RaiseWindow(window); }
	void Hide() { SDL_HideWindow(window); }
	void Show() { SDL_ShowWindow(window); }
	bool GLSwap() { return SDL_GL_SwapWindow(window); }
	SDL_Window* GetSDLWindow() const { return window; }
	SDL_GLContext GetGLContext() const { return glContext; }
	int Width() const { return width; }
	int Height() const { return height; }
	bool IsFullscreen() const { return is_fullscreen; }
	bool Is3D() const { return is3D; }
	void Destroy() const;
	const GLuint& GetFBO() const { return fbo; }
	GLuint64 GetTextureHandle() const { return texture_handle; }
	void Clear(SDL_Color sys_col);
	void Reset3D();

	void DrawToDefault();
};

#endif