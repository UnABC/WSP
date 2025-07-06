#include <GL/glew.h>
#include "graphic.hpp"
using namespace std;

Graphic::Graphic(int width, int height, bool is_fullscreen) : width(width), height(height), is_fullscreen(is_fullscreen), colors(4)
{
	//色々初期化
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
		throw WindowException("Failed to initialize SDL.");
	//ウィンドウ作成
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 5);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	window = SDL_CreateWindow("WSP", 640, 480, (is_fullscreen ? SDL_WINDOW_FULLSCREEN : SDL_WINDOW_HIDDEN) | SDL_WINDOW_OPENGL);
	if (!window)FailedToInitialize("Failed to create SDL Window.");
	//レンダラー作成
	renderer = SDL_CreateRenderer(window, NULL);
	if (!renderer)FailedToInitialize("Failed to create SDL Renderer.");
	//OpenGLの初期化
	glContext = SDL_GL_CreateContext(window);
	if (!glContext)FailedToInitialize("Failed to create OpenGL context.");
	if (SDL_GL_MakeCurrent(window, glContext) < 0)
		FailedToInitialize("Failed to make OpenGL context current.");
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_DEPTH_TEST);

	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0.0, width, height, 0.0, -1.0, 1.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	//OpenGLの有効化
	if (SDL_GL_MakeCurrent(window, glContext) < 0)
		FailedToInitialize("Failed to make OpenGL context current.");
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	if (!SDL_GL_SwapWindow(window))
		FailedToInitialize(SDL_GetError());
	glewExperimental = GL_TRUE;
	GLenum err = glewInit();
	if (err != GLEW_OK)
		throw WindowException(reinterpret_cast<const char*>(glewGetErrorString(err)));
	if (!GLEW_ARB_bindless_texture)
		throw WindowException("OpenGL does not support bindless textures.");
	//フォント初期化+読み込み
	font.Init(width, height);
	font.SetFont("C:\\Windows\\Fonts\\msgothic.ttc", font_size);
	//各種図形の初期化
	shape.Init(width, height);
	//画像の初期化
	image.Init(width, height);

	//開始時刻記録
	lastTime = SDL_GetTicks();

	//リフレッシュレート取得&FPS設定
	int displayCount = 0;
	SDL_DisplayID* displays = SDL_GetDisplays(&displayCount);
	if (!displays || displayCount < 1) {
		SDL_free(displays);
		FailedToInitialize("Failed to get display information.");
	}
	SDL_DisplayID primary_display_id = displays[0];
	SDL_free(displays);
	const SDL_DisplayMode* displayMode = SDL_GetCurrentDisplayMode(primary_display_id);
	if (!displayMode)
		FailedToInitialize(SDL_GetError());
	fps = max(displayMode->refresh_rate, 24.0F); // ディスプレイのリフレッシュレートをFPSに設定
	//ウィンドウ表示
	SDL_ShowWindow(window);
}

void Graphic::FailedToInitialize(const string& message) {
	End();
	throw WindowException(message);
}

void Graphic::SetColors(int r, int g, int b, int a, int index) {
	if (index < 0 || index >= colors.size()) return; //範囲外アクセス防止
	colors.at(index) = { (Uint8)r, (Uint8)g, (Uint8)b,(Uint8)a };
	if (index == 0) color = colors.at(0); //0番目の色を現在の色に設定
}

void Graphic::SetFont(unsigned long long size, const string& font_path) {
	font.SetFont(font_path.c_str(), size);
	font_size = size;
}

void Graphic::printText(const string& text) {
	current_depth += depth_increment; // 深度を更新
	font.SetTexts(text, pos.x, pos.y, 1.0f, width, colors.at(0), colors.at(1), colors.at(2), colors.at(3), current_depth, gmode);
	Draw();
	//下にずらす
	pos.y += font_size * (count(text.begin(), text.end(), '\n') + 1);
}

void Graphic::Load_Image(const string& file_path, unsigned int id, int center_x, int center_y) {
	image.Load(file_path, id, center_x, center_y);
}

//ダイアログ表示
//type: 0=情報, 1=警告, 2=エラー
void Graphic::CallDialog(const string& title, const string& message, int type) const {
	SDL_MessageBoxFlags flags;
	switch (type) {
	case 0: flags = SDL_MESSAGEBOX_INFORMATION; break;
	case 1: flags = SDL_MESSAGEBOX_WARNING; break;
	case 2: flags = SDL_MESSAGEBOX_ERROR; break;
	default: flags = SDL_MESSAGEBOX_INFORMATION; break; //デフォルトは情報
	}
	SDL_ShowSimpleMessageBox(flags, title.c_str(), message.c_str(), window);
}

void Graphic::DrawTriangle(float x1, float y1, float x2, float y2, float x3, float y3) {
	current_depth += depth_increment; // 深度を更新
	shape.draw_triangle(x1, y1, x2, y2, x3, y3, colors.at(0), colors.at(1), colors.at(2), current_depth, gmode);
	Draw();
}

void Graphic::DrawRectangle(float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4) {
	current_depth += depth_increment; // 深度を更新
	shape.draw_triangle(x1, y1, x2, y2, x3, y3, colors.at(0), colors.at(1), colors.at(2), current_depth, gmode);
	shape.draw_triangle(x1, y1, x3, y3, x4, y4, colors.at(0), colors.at(2), colors.at(3), current_depth, gmode);
	Draw();
}

void Graphic::DrawRoundRect(float x, float y, float width, float height, float radius) {
	current_depth += depth_increment; // 深度を更新
	shape.draw_round_rectangle(x, y, width, height, radius, colors.at(0), colors.at(1), colors.at(2), colors.at(3), current_depth, gmode);
	Draw();
}

void Graphic::DrawLine(float x1, float y1, float x2, float y2) {
	current_depth += depth_increment; // 深度を更新
	shape.draw_line(x1, y1, x2, y2, colors.at(0), colors.at(1), current_depth, gmode);
	Draw();
	pos.x = x1; // 最後の座標を表示位置に設定
	pos.y = y1; // 最後の座標を表示位置に設定
}

void Graphic::DrawEllipse(float center_x, float center_y, float major_axis, float minor_axis, float angle) {
	current_depth += depth_increment; // 深度を更新
	shape.draw_ellipse(center_x, center_y, major_axis, minor_axis, -angle, colors.at(0), colors.at(1), colors.at(2), colors.at(3), current_depth, gmode);
	Draw();
}

void Graphic::DrawImage(unsigned int id, float x_size, float y_size, float angle) {
	current_depth += depth_increment; // 深度を更新
	if (gmode & 1) {
		image.DrawImage(id, pos.x, pos.y, x_size, y_size, -angle, colors.at(0), colors.at(1), colors.at(2), colors.at(3), current_depth, gmode);
	} else {
		SDL_Color image_color = { 255, 255, 255, 255 }; // デフォルトの画像色
		image.DrawImage(id, pos.x, pos.y, x_size, y_size, -angle, image_color, image_color, image_color, image_color, current_depth, gmode);
	}
	Draw();
}

void Graphic::Clear(int r, int g, int b) {
	//テクスチャのクリア
	font.Clear();
	shape.Clear();
	image.Clear();
	pos = { 0, 0 };	//表示位置を初期化
	current_depth = 0.0f; //深度を初期化
	system_color = { (Uint8)r, (Uint8)g, (Uint8)b, 255 }; //システム色を設定
	color = { 255,255,255, 255 };	//色を初期化
	fill(colors.begin(), colors.end(), color); //色のリストを現在の色で埋める
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	gmode = 0; //描画モードを初期化
}

void Graphic::Draw() {
	if (!redraw) return; //描画フラグがfalseなら何もしない
	if (lastTime + 1000 / fps > SDL_GetTicks()) return; //フレームレート制限
	lastTime = SDL_GetTicks();

	glClearColor(system_color.r / 255.0f,
		system_color.g / 255.0f,
		system_color.b / 255.0f,
		system_color.a / 255.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // カラーバッファと深度バッファをクリア
	//テクスチャの描画
	font.DrawTexts();
	//図形の描画
	shape.draw_shapes();
	shape.draw_roundrect();
	shape.draw_lines();
	shape.draw_ellipses();
	//画像の描画
	image.Draw();

	if (!SDL_GL_SwapWindow(window))
		FailedToInitialize(SDL_GetError());
}

bool Graphic::Wait(unsigned long long milliseconds) {
	unsigned long long start = SDL_GetTicks();
	while (SDL_GetTicks() - start < milliseconds) {
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_EVENT_QUIT) return false; // ウィンドウが閉じられた場合はfalseを返す
		}
		SDL_Delay(1); // CPU使用率を下げるために少し待機
	}
	return true;
}

void Graphic::Stop() {
	bool running = true;
	while (running) {
		while (SDL_PollEvent(&event)) if (event.type == SDL_EVENT_QUIT) running = false;
		Draw();
	}
	End();
}

void Graphic::End() const {
	//ウィンドウを閉じる
	//if (face)FT_Done_Face(face);
	//if (library)FT_Done_FreeType(library);
	SDL_GL_DestroyContext(glContext);
	SDL_DestroyWindow(window);
	SDL_DestroyRenderer(renderer);
	SDL_Quit();
	//TTF_Quit();
}

