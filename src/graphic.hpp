#ifndef GRAPHIC_HPP
#define GRAPHIC_HPP

#include <SDL3/SDL.h>
#include <SDL3/SDL_opengl.h>
#include <SDL3/SDL_ttf.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "exception.hpp"
#include "font.hpp"
#include <vector>
#include <map>

struct Position {
	float x, y;
	Position(float x, float y) : x(x), y(y) {};
};

class Graphic {
private:
	//ウィンドウ状態
	int width, height;
	bool is_fullscreen;
	//各種システム変数
	SDL_Color color = { 0, 0, 0, 255 };	//白色
	Position pos = { 0, 0 };	//表示位置
	unsigned long long font_size = 24;	//フォントサイズ
	bool redraw = true;	//描画フラグ

	//描画するテクスチャ達
	std::vector<std::tuple<SDL_Texture*,SDL_FRect,SDL_FRect>> textures;

	//キャッシュ
	std::map<std::string, SDL_Texture*> text_cache;
	//SDL variables
	SDL_Window* window;
	//SDL_Renderer *renderer;
	//TTF_Font* font;
	FT_Face face;
	FT_Library library;
	Font font;
	SDL_GLContext glContext;

	void FailedToInitialize(const std::string& message);
	//SDL_Texture* CashText(const std::string& text);
public:
	Graphic(int width = 640, int height = 480,bool is_fullscreen = false);

	void printText(const std::string& text);
	void SetPos(float x, float y) { pos.x = x; pos.y = y; };
	void SetColor(int r, int g, int b) { color = { (Uint8)r, (Uint8)g, (Uint8)b, 255 }; };
	void SetFontSize(unsigned long long size) { font_size = size; };
	void Clear();

	void Draw();
	void Stop();
	void End() const;
};
#endif

