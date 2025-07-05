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
#include "Image.hpp"
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
	int gmode = 0;	//描画モード（0:通常, 1:画像color適用, 2:加算合成,4:減算合成,6:乗算合成）

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
	Image image;
	SDL_GLContext glContext;

	void FailedToInitialize(const std::string& message);
public:
	Graphic(int width = 640, int height = 480, bool is_fullscreen = false);

	void printText(const std::string& text);
	void SetPos(float x, float y) { pos.x = x; pos.y = y; };
	void SetColor(int r, int g, int b,int a = 255) { color = { (Uint8)r, (Uint8)g, (Uint8)b, (Uint8)a };std::fill(colors.begin(), colors.end(), color); };
	void SetColors(int r, int g, int b, int a, int index);
	void SetFontSize(unsigned long long size) { font_size = size; };
	void SetFont(unsigned long long size = 24, const std::string& font_path = "C:/Windows/Fonts/msgothic.ttc");
	void Load_Image(const std::string& file_path, unsigned int id, int center_x = 0, int center_y = 0);
	void SetGmode(int mode){gmode = mode;}

	void CallDialog(const std::string& title, const std::string& message, int type = 0) const;
	bool Wait(unsigned long long milliseconds = 1);
	void DrawTriangle(float x1, float y1, float x2, float y2, float x3, float y3);
	void DrawRectangle(float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4);
	void DrawRoundRect(float x, float y, float width, float height, float radius);
	void DrawLine(float x1, float y1, float x2, float y2);
	void DrawLine(float x1, float y1) { DrawLine(x1, y1, pos.x, pos.y); };
	void DrawEllipse(float center_x, float center_y, float major_axis, float minor_axis, float angle = 0.0f);

	void DrawImage(unsigned int id, float x_size = 1.0f, float y_size = 1.0f, float angle = 0.0f);

	void Clear(int r = 255, int g = 255, int b = 255);
	void SetRedraw(bool redraw) { if (this->redraw = redraw)Draw(); }

	void Draw();
	void Stop();
	void End() const;
};
#endif

