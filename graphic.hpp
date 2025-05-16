#ifndef GRAPHIC_HPP
#define GRAPHIC_HPP

#include <SDL3/SDL.h>
#include <SDL3/SDL_opengl.h>
#include <SDL3/SDL_ttf.h>
#include "exception.hpp"
#include <vector>

struct Position {
	float x, y;
	Position(float x, float y) : x(x), y(y) {};
};

class Graphic {
private:
	//ウィンドウ状態
	int width, height;
	bool is_fullscreen;
	//各種内部保持変数
	SDL_Color color = { 255, 255, 255, 255 };	//白色
	Position pos = { 0, 0 };	//表示位置
	unsigned long long font_size = 24;	//フォントサイズ

	//描画するテクスチャ達
	std::vector<std::tuple<SDL_Texture*,SDL_FRect,SDL_FRect>> textures;

	//SDL variables
	SDL_Window* window;
	SDL_Renderer *renderer;
	TTF_Font* font;

	void FailedToInitialize(const std::string& message);
public:
	Graphic(int width = 640, int height = 480,bool is_fullscreen = false);

	void printText(const std::string& text);
	void SetPos(float x, float y) { pos.x = x; pos.y = y; };

	void Draw() const;
	void Stop();
};
#endif

