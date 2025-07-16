#ifndef GRAPHIC_HPP
#define GRAPHIC_HPP

#include "exception.hpp"
#include "font.hpp"
#include "Image.hpp"
#include "window.hpp"

#include <SDL3/SDL.h>
#include <SDL3/SDL_opengl.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <vector>
#include <map>
#include <ranges>
#include <iostream>

class Graphic {
private:
	//ウィンドウ状態
	std::map<int, Window> windows;
	int WinID = 0;	//現在のウィンドウID

	unsigned long long font_size = 24;	//フォントサイズ
	bool redraw = true;	//描画フラグ
	int gmode = 0;	//描画モード（0:通常, 1:画像color適用, 2:加算合成,4:減算合成,6:乗算合成）

	//各種内部変数
	float fps = 60.0;	//フレームレート
	unsigned long long lastTime = 0;	//タイマー
	unsigned long long awaitTime = 0;

	SDL_Event event;
	// 画像データのリスト
	std::map<unsigned int, image_data> images;
	// fontキャッシュ
	std::unordered_map<char16_t, Character> characters;
	BLTexture glyph_atlas;

	void FailedToInitialize(const std::string& message);
	void Set_Gmode(int gmode);
	SDL_Color HSV2RGB(int h, int s, int v) const;
public:
	Graphic(int width = 640, int height = 480, bool is_fullscreen = false);

	void printText(const std::string& text);
	void SetPos(float x, float y, float z = 0.0f) { windows[WinID].pos.x = x; windows[WinID].pos.y = y;windows[WinID].pos.z = z; };
	void SetColor(int r, int g, int b, int a = 255) { windows[WinID].color = { (Uint8)r, (Uint8)g, (Uint8)b, (Uint8)a };std::fill(windows[WinID].colors.begin(), windows[WinID].colors.end(), windows[WinID].color); };
	void SetHSVColor(int h, int s, int v, int a = 255) { windows[WinID].color = HSV2RGB(h, s, v); windows[WinID].color.a = (Uint8)a; std::fill(windows[WinID].colors.begin(), windows[WinID].colors.end(), windows[WinID].color); };
	void SetColors(int r, int g, int b, int a, int index);
	void SetHSVColors(int h, int s, int v, int a, int index);
	void SetFontSize(unsigned long long size) { font_size = size; };
	void SetFont(unsigned long long size = 24, const std::string& font_path = "C:/Windows/Fonts/msgothic.ttc");
	void Load_Image(const std::string& file_path, unsigned int id, int center_x = 0, int center_y = 0);
	void SetGmode(int mode) { gmode = mode; }
	void SetTexture(int id, float tex_x = 0.0f, float tex_y = 0.0f, float tex_width = -1.0f, float tex_height = -1.0f);

	void CallDialog(const std::string& title, const std::string& message, int type = 0) const;
	bool Wait(unsigned long long milliseconds = 1);
	bool AWait(unsigned long long milliseconds = 1);
	void DrawTriangle(float x1, float y1, float x2, float y2, float x3, float y3);
	void DrawRectangle(float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4);
	void DrawRoundRect(float x, float y, float width, float height, float radius);
	void DrawLine(float x1, float y1, float x2, float y2);
	void DrawLine(float x1, float y1) { DrawLine(x1, y1, windows[WinID].pos.x, windows[WinID].pos.y); };
	void DrawEllipse(float center_x, float center_y, float major_axis, float minor_axis, float angle = 0.0f);

	void DrawImage(unsigned int id, float x_size = 1.0f, float y_size = 1.0f, float angle = 0.0f, int tex_x1 = -1, int tex_y1 = -1, int tex_width = -1, int tex_height = -1);

	void Clear(int r = 255, int g = 255, int b = 255);
	void Reset3D(){ windows[WinID].Reset3D(); }
	void SetRedraw(bool redraw) { if (this->redraw = redraw)Draw(); }

	void CreateScreen(int id, const std::string& title = "WSP", int width = 640, int height = 480, int mode = 0);
	void MakeCurrentWindow(int id, int mode = 0);
	void HideWindow(int id);
	void ShowWindow(int id);
	void DestroyWindow(int id);
	void ResizeWindow(int new_width, int new_height);
	void SetWindowTitle(const std::string& title) const;
	void SetWindowPosition(int id, int x, int y);
	void SetCameraPos(float x = 0.0f, float y = 0.0f, float z = 0.0f, float target_x = 0.0f, float target_y = 0.0f, float target_z = 0.0f);

	void Gcopy(int id, int src_x = 0, int src_y = 0, int src_width = -1, int src_height = -1, int dst_x = 0, int dst_y = 0, int dst_width = -1, int dst_height = -1);

	void Draw(bool force = false);
	void Stop();
	void End() const;
};
#endif

