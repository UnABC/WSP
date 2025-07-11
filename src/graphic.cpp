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
	Clear();
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

void Graphic::SetHSVColors(int h, int s, int v, int a, int index) {
	if (index < 0 || index >= colors.size()) return; //範囲外アクセス防止
	colors.at(index) = HSV2RGB(h, s, v);
	colors.at(index).a = (Uint8)a;
	if (index == 0) color = colors.at(0); //0番目の色を現在の色に設定
}

void Graphic::SetFont(unsigned long long size, const string& font_path) {
	font.SetFont(font_path.c_str(), size);
	font_size = size;
}

void Graphic::printText(const string& text) {
	font.SetTexts(text, pos.x, pos.y, width, colors.at(0), colors.at(1), colors.at(2), colors.at(3), gmode, all_vertices);
	Draw();
	//下にずらす
	pos.y += font_size * (count(text.begin(), text.end(), '\n') + 1);
}

void Graphic::Load_Image(const string& file_path, unsigned int id, int center_x, int center_y) {
	image.Load(file_path, id, center_x, center_y);
}

void Graphic::SetTexture(int id, float tex_x, float tex_y, float tex_width, float tex_height) {
	if (id < 0) {
		shape.SetTexture(0, false); //無効なIDならテクスチャを無効化
		return;
	}
	shape.SetTexture(image.GetImageData(id), true, tex_x, tex_y, tex_width, tex_height);
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
	shape.draw_triangle(x1, y1, x2, y2, x3, y3, colors.at(0), colors.at(1), colors.at(2), gmode, all_vertices, 0);
	Draw();
}

void Graphic::DrawRectangle(float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4) {
	shape.draw_triangle(x1, y1, x2, y2, x3, y3, colors.at(0), colors.at(1), colors.at(2), gmode, all_vertices, 1);
	shape.draw_triangle(x1, y1, x3, y3, x4, y4, colors.at(0), colors.at(2), colors.at(3), gmode, all_vertices, 2);
	Draw();
}

void Graphic::DrawRoundRect(float x, float y, float width, float height, float radius) {
	shape.draw_round_rectangle(x, y, width, height, radius, colors.at(0), colors.at(1), colors.at(2), colors.at(3), gmode, all_vertices);
	Draw();
}

void Graphic::DrawLine(float x1, float y1, float x2, float y2) {
	shape.draw_line(x1, y1, x2, y2, colors.at(0), colors.at(1), gmode, all_vertices);
	Draw();
	pos.x = x1; // 最後の座標を表示位置に設定
	pos.y = y1; // 最後の座標を表示位置に設定
}

void Graphic::DrawEllipse(float center_x, float center_y, float major_axis, float minor_axis, float angle) {
	shape.draw_ellipse(center_x, center_y, major_axis, minor_axis, -angle, colors.at(0), colors.at(1), colors.at(2), colors.at(3), gmode, all_vertices);
	Draw();
}

void Graphic::DrawImage(unsigned int id, float x_size, float y_size, float angle, int tex_x, int tex_y, int tex_width, int tex_height) {
	if (gmode & 1) {
		image.DrawImage(id, pos.x, pos.y, x_size, y_size, -angle, tex_x, tex_y, tex_width, tex_height, colors.at(0), colors.at(1), colors.at(2), colors.at(3), gmode, all_vertices);
	} else {
		SDL_Color image_color = { 255, 255, 255, 255 }; // デフォルトの画像色
		image.DrawImage(id, pos.x, pos.y, x_size, y_size, -angle, tex_x, tex_y, tex_width, tex_height, image_color, image_color, image_color, image_color, gmode, all_vertices);
	}
	Draw();
}

void Graphic::Clear(int r, int g, int b) {
	all_vertices.clear();
	//テクスチャのクリア
	pos = { 0, 0 };	//表示位置を初期化
	system_color = { (Uint8)r, (Uint8)g, (Uint8)b, 255 }; //システム色を設定
	color = { 0,0,0, 255 };	//色を初期化
	fill(colors.begin(), colors.end(), color); //色のリストを現在の色で埋める
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	shape.SetTexture(0, false);
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
	//各種描画
	int old_id = -1;
	for (auto& vertex_data : all_vertices) {
		if (vertex_data.ID != old_id) {
			if (!vertex_data.shaderProgram || !vertex_data.vao || !vertex_data.vbo)
				throw WindowException("Invalid vertex data in all_vertices.");
			glUseProgram(vertex_data.shaderProgram);
			glBindVertexArray(vertex_data.vao);
			glUniformMatrix4fv(glGetUniformLocation(vertex_data.shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(vertex_data.projection));
			glBindBuffer(GL_ARRAY_BUFFER, vertex_data.vbo);
		}
		Set_Gmode(vertex_data.gmode);
		glBufferData(GL_ARRAY_BUFFER, vertex_data.all_vertices.size() * sizeof(float), vertex_data.all_vertices.data(), GL_DYNAMIC_DRAW);
		glDrawArrays(vertex_data.graphics_mode, 0, vertex_data.all_vertices.size() / vertex_data.division);
		old_id = vertex_data.ID; // 最後に描画したIDを保存
	}
	glBindVertexArray(0);

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

bool Graphic::AWait(unsigned long long milliseconds) {
	do {
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_EVENT_QUIT) return false; // ウィンドウが閉じられた場合はfalseを返す
		}
		SDL_Delay(1); // CPU使用率を下げるために少し待機
	} while (SDL_GetTicks() - awaitTime < milliseconds);
	awaitTime = SDL_GetTicks();
	return true;
}

void Graphic::Stop() {
	bool running = true;
	while (running) {
		while (SDL_PollEvent(&event)) if (event.type == SDL_EVENT_QUIT) running = false;
		Draw();
		SDL_Delay(16);
	}
	End();
}

void Graphic::End() const {
	//ウィンドウを閉じる
	SDL_GL_DestroyContext(glContext);
	SDL_DestroyWindow(window);
	SDL_DestroyRenderer(renderer);
	SDL_Quit();
}

void Graphic::Set_Gmode(int gmode) {
	if (gmode == 0) {
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	} else if (gmode == 2) {
		// 加算合成
		glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	} else if (gmode == 4) {
		// 減算合成
		glBlendFunc(GL_ZERO, GL_ONE_MINUS_SRC_ALPHA);
	} else if (gmode == 6) {
		// 乗算合成
		glBlendFunc(GL_DST_ALPHA, GL_ZERO);
	}
}

SDL_Color Graphic::HSV2RGB(int h, int s, int v) const {
	float r, g, b;
	float f, p, q, t;
	int i;

	if (s == 0) {
		r = g = b = v / 255.0f; // グレースケール
	} else {
		h /= 60; // 色相を0-5の範囲に変換
		i = static_cast<int>(h);
		f = h - i; // 小数部分
		p = v * (1 - s / 255.0f);
		q = v * (1 - f * s / 255.0f);
		t = v * (1 - (1 - f) * s / 255.0f);

		switch (i) {
		case 0: r = v / 255.0f; g = t / 255.0f; b = p / 255.0f; break;
		case 1: r = q / 255.0f; g = v / 255.0f; b = p / 255.0f; break;
		case 2: r = p / 255.0f; g = v / 255.0f; b = t / 255.0f; break;
		case 3: r = p / 255.0f; g = q / 255.0f; b = v / 255.0f; break;
		case 4: r = t / 255.0f; g = p / 255.0f; b = v / 255.0f; break;
		default: r = v / 255.0f; g = p / 255.0f; b = q / 255.0f; break;
		}
	}

	return { static_cast<Uint8>(r * 255), static_cast<Uint8>(g * 255), static_cast<Uint8>(b * 255), 255 };
}

void Graphic::ResizeWindow(int new_width, int new_height) {
	if (new_width <= 0 || new_height <= 0) return; // 無効なサイズは無視
	width = new_width;
	height = new_height;
	SDL_SetWindowSize(window, width, height);
	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0.0, width, height, 0.0, -1.0, 1.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	shape.updateProjection(width, height);
	image.updateProjection(width, height);
	font.updateProjection(width, height);
}
void Graphic::SetWindowTitle(const std::string& title) const {
	if (SDL_SetWindowTitle(window, title.c_str()) < 0) {
		throw WindowException("Failed to set window title: " + std::string(SDL_GetError()));
	}
}