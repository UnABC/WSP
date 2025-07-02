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

	float current_depth = 0.0f;	//現在の深度
	const float depth_increment = 0.0000001f;	//深度の増分

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
	void SetFont(unsigned long long size = 24, const std::string& font_path = "C:/Windows/Fonts/msgothic.ttc");
	void CallDialog(const std::string& title, const std::string& message, int type = 0) const;
	bool Wait(unsigned long long milliseconds = 1);
	void DrawTriangle(float x1, float y1, float x2, float y2, float x3, float y3);
	void DrawRectangle(float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4);
	void DrawRoundRect(float x, float y, float width, float height, float radius);
	void DrawLine(float x1, float y1, float x2, float y2);
	void DrawLine(float x1, float y1){DrawLine(x1, y1, pos.x, pos.y);};
	void Clear(int r = 255, int g = 255, int b = 255);
	void SetRedraw(bool redraw) { if (this->redraw = redraw)Draw(); }

	void Draw();
	void Stop();
	void End() const;
};
#endif

