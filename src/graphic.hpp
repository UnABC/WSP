#ifndef GRAPHIC_HPP
#define GRAPHIC_HPP

#include <SDL3/SDL.h>
#include <SDL3/SDL_opengl.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "exception.hpp"
#include "font.hpp"
#include "shape.hpp"
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
	SDL_Color color = { 0, 0, 0, 255 };	//黒色
	SDL_Color system_color = { 255, 255, 255, 255 };	//システム色（白色）
	std::vector<SDL_Color> colors;	//色のリスト
	Position pos = { 0, 0 };	//表示位置
	unsigned long long font_size = 24;	//フォントサイズ
	bool redraw = true;	//描画フラグ

	//各種内部変数
	float fps = 60.0;	//フレームレート
	unsigned long long lastTime = 0;	//タイマー
	SDL_Event event;
	//描画するテクスチャ達
	std::vector<std::tuple<SDL_Texture*, SDL_FRect, SDL_FRect>> textures;

	//キャッシュ
	std::map<std::string, SDL_Texture*> text_cache;
	SDL_Window* window;
	SDL_Renderer* renderer;
	Font font;
	Shape shape;
	SDL_GLContext glContext;

	void FailedToInitialize(const std::string& message);
public:
	Graphic(int width = 640, int height = 480, bool is_fullscreen = false);

	void printText(const std::string& text);
	void SetPos(float x, float y) { pos.x = x; pos.y = y; };
	void SetColor(int r, int g, int b) { color = { (Uint8)r, (Uint8)g, (Uint8)b, 255 };std::fill(colors.begin(), colors.end(), color); };
	void SetColors(int r, int g, int b, int index);
	void SetFontSize(unsigned long long size) { font_size = size; };
	void CallDialog(const std::string& title, const std::string& message, int type = 0) const;
	bool Wait(unsigned long long milliseconds = 1);
	void DrawTriangle(double x1, double y1, double x2, double y2, double x3, double y3);
	void Clear(int r = 255, int g = 255, int b = 255);

	void Draw();
	void Stop();
	void End() const;
};
#endif

