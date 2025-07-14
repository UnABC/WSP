#include <GL/glew.h>
#include "graphic.hpp"
using namespace std;

Graphic::Graphic(int width, int height, bool is_fullscreen)
{
	//色々初期化
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
		throw WindowException("Failed to initialize SDL.");
	//ウィンドウ作成
	windows[WinID].Create(true, "WSP", width, height, is_fullscreen);
	//フォント初期化+読み込み
	windows[WinID].font.Init(characters, glyph_atlas, &windows[WinID].projection);
	glyph_atlas.create(2048, 2048, GL_R8, GL_RED);
	windows[WinID].font.SetFont("C:\\Windows\\Fonts\\msgothic.ttc", font_size);
	//各種図形の初期化
	windows[WinID].shape.Init(width, height, &windows[WinID].projection);
	//画像の初期化
	windows[WinID].image.Init(images, &windows[WinID].projection);

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
	windows[WinID].Show();
}

void Graphic::FailedToInitialize(const string& message) {
	End();
	throw WindowException(message);
}

void Graphic::SetColors(int r, int g, int b, int a, int index) {
	if (index < 0 || index >= windows[WinID].colors.size()) return; //範囲外アクセス防止
	windows[WinID].colors.at(index) = { (Uint8)r, (Uint8)g, (Uint8)b,(Uint8)a };
	if (index == 0) windows[WinID].color = windows[WinID].colors.at(0); //0番目の色を現在の色に設定
}

void Graphic::SetHSVColors(int h, int s, int v, int a, int index) {
	if (index < 0 || index >= windows[WinID].colors.size()) return; //範囲外アクセス防止
	windows[WinID].colors.at(index) = HSV2RGB(h, s, v);
	windows[WinID].colors.at(index).a = (Uint8)a;
	if (index == 0) windows[WinID].color = windows[WinID].colors.at(0); //0番目の色を現在の色に設定
}

void Graphic::SetFont(unsigned long long size, const string& font_path) {
	windows[WinID].font.SetFont(font_path.c_str(), size);
	font_size = size;
}

void Graphic::printText(const string& text) {
	windows[WinID].font.SetTexts(text, windows[WinID].pos.x, windows[WinID].pos.y, windows[WinID].Width(), windows[WinID].colors.at(0), windows[WinID].colors.at(1), windows[WinID].colors.at(2), windows[WinID].colors.at(3), gmode, windows[WinID].all_vertices);
	Draw();
	//下にずらす
	windows[WinID].pos.y += font_size * (count(text.begin(), text.end(), '\n') + 1);
}

void Graphic::Load_Image(const string& file_path, unsigned int id, int center_x, int center_y) {
	windows[WinID].image.Load(file_path, id, center_x, center_y);
}

void Graphic::SetTexture(int id, float tex_x, float tex_y, float tex_width, float tex_height) {
	if (id < 0) {
		windows[WinID].shape.SetTexture(0, false); //無効なIDならテクスチャを無効化
		return;
	}
	windows[WinID].shape.SetTexture(&images[id], true, tex_x, tex_y, tex_width, tex_height);
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
	SDL_ShowSimpleMessageBox(flags, title.c_str(), message.c_str(), windows.at(WinID).GetSDLWindow());
}

void Graphic::DrawTriangle(float x1, float y1, float x2, float y2, float x3, float y3) {
	windows[WinID].shape.draw_triangle(x1, y1, x2, y2, x3, y3, windows[WinID].colors.at(0), windows[WinID].colors.at(1), windows[WinID].colors.at(2), gmode, windows[WinID].all_vertices, 0);
	Draw();
}

void Graphic::DrawRectangle(float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4) {
	windows[WinID].shape.draw_triangle(x1, y1, x2, y2, x3, y3, windows[WinID].colors.at(0), windows[WinID].colors.at(1), windows[WinID].colors.at(2), gmode, windows[WinID].all_vertices, 1);
	windows[WinID].shape.draw_triangle(x1, y1, x3, y3, x4, y4, windows[WinID].colors.at(0), windows[WinID].colors.at(2), windows[WinID].colors.at(3), gmode, windows[WinID].all_vertices, 2);
	Draw();
}

void Graphic::DrawRoundRect(float x, float y, float width, float height, float radius) {
	windows[WinID].shape.draw_round_rectangle(x, y, width, height, radius, windows[WinID].colors.at(0), windows[WinID].colors.at(1), windows[WinID].colors.at(2), windows[WinID].colors.at(3), gmode, windows[WinID].all_vertices);
	Draw();
}

void Graphic::DrawLine(float x1, float y1, float x2, float y2) {
	windows[WinID].shape.draw_line(x1, y1, x2, y2, windows[WinID].colors.at(0), windows[WinID].colors.at(1), gmode, windows[WinID].all_vertices);
	Draw();
	windows[WinID].pos.x = x1; // 最後の座標を表示位置に設定
	windows[WinID].pos.y = y1; // 最後の座標を表示位置に設定
}

void Graphic::DrawEllipse(float center_x, float center_y, float major_axis, float minor_axis, float angle) {
	windows[WinID].shape.draw_ellipse(center_x, center_y, major_axis, minor_axis, -angle, windows[WinID].colors.at(0), windows[WinID].colors.at(1), windows[WinID].colors.at(2), windows[WinID].colors.at(3), gmode, windows[WinID].all_vertices);
	Draw();
}

void Graphic::DrawImage(unsigned int id, float x_size, float y_size, float angle, int tex_x, int tex_y, int tex_width, int tex_height) {
	if (gmode & 1) {
		windows[WinID].image.DrawImage(id, windows[WinID].pos.x, windows[WinID].pos.y, x_size, y_size, -angle, tex_x, tex_y, tex_width, tex_height, windows[WinID].colors.at(0), windows[WinID].colors.at(1), windows[WinID].colors.at(2), windows[WinID].colors.at(3), gmode, windows[WinID].all_vertices);
	} else {
		SDL_Color image_color = { 255, 255, 255, 255 }; // デフォルトの画像色
		windows[WinID].image.DrawImage(id, windows[WinID].pos.x, windows[WinID].pos.y, x_size, y_size, -angle, tex_x, tex_y, tex_width, tex_height, image_color, image_color, image_color, image_color, gmode, windows[WinID].all_vertices);
	}
	Draw();
}

void Graphic::Clear(int r, int g, int b) {
	windows[WinID].all_vertices.clear();
	//テクスチャのクリア
	windows[WinID].pos = { 0, 0 };	//表示位置を初期化
	windows[WinID].system_color = { (Uint8)r, (Uint8)g, (Uint8)b, 255 }; //システム色を設定
	windows[WinID].color = { 0,0,0, 255 };	//色を初期化
	fill(windows[WinID].colors.begin(), windows[WinID].colors.end(), windows[WinID].color); //色のリストを現在の色で埋める
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	windows[WinID].shape.SetTexture(0, false);
	gmode = 0; //描画モードを初期化
}

void Graphic::Draw(bool force) {
	if (!redraw) return; //描画フラグがfalseなら何もしない
	if (!force && (lastTime + 1000 / fps > SDL_GetTicks())) return; //フレームレート制限
	lastTime = SDL_GetTicks();

	glBindFramebuffer(GL_FRAMEBUFFER, windows[WinID].GetFBO()); // FBOに描画
	glViewport(0, 0, windows[WinID].Width(), windows[WinID].Height()); // ビューポートをウィンドウサイズに設定
	// 背景色を設定
	glClearColor(windows[WinID].system_color.r / 255.0f,
		windows[WinID].system_color.g / 255.0f,
		windows[WinID].system_color.b / 255.0f,
		windows[WinID].system_color.a / 255.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // カラーバッファと深度バッファをクリア
	//各種描画
	int old_id = -1;
	for (auto& vertex_data : windows[WinID].all_vertices) {
		if (vertex_data.ID == 6) {
			old_id = 6; // FBOの描画は特別扱い
			int id = vertex_data.division; // FBOのIDを取得
			glBindFramebuffer(GL_READ_FRAMEBUFFER, windows[id].GetFBO());
			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, windows[WinID].GetFBO());
			glBlitFramebuffer(vertex_data.all_vertices[0], vertex_data.all_vertices[1],
				vertex_data.all_vertices[2], vertex_data.all_vertices[3],
				vertex_data.all_vertices[4], vertex_data.all_vertices[5],
				vertex_data.all_vertices[6], vertex_data.all_vertices[7],
				GL_COLOR_BUFFER_BIT, GL_LINEAR);
			glBindFramebuffer(GL_FRAMEBUFFER, windows[WinID].GetFBO());
			continue; // FBOの描画はここで終了
		}
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

	windows[WinID].DrawToDefault(); // スクリーン全体を描画

	if (!windows[WinID].GLSwap())
		FailedToInitialize(SDL_GetError());
}

bool Graphic::Wait(unsigned long long milliseconds) {
	unsigned long long start = SDL_GetTicks();
	while (SDL_GetTicks() - start < milliseconds) {
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_EVENT_QUIT) return false;
			if (event.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED)
				return false; // ウィンドウが閉じられた場合はfalseを返す
		}
		SDL_Delay(1); // CPU使用率を下げるために少し待機
	}
	return true;
}

bool Graphic::AWait(unsigned long long milliseconds) {
	do {
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_EVENT_QUIT) return false;
			if (event.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED)
				return false; // ウィンドウが閉じられた場合はfalseを返す
		}
		SDL_Delay(1); // CPU使用率を下げるために少し待機
	} while (SDL_GetTicks() - awaitTime < milliseconds);
	awaitTime = SDL_GetTicks();
	return true;
}

void Graphic::Stop() {
	bool running = true;
	while (running) {
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_EVENT_QUIT) running = false;
			if (event.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED)
				running = false; // ウィンドウが閉じられた場合はループを終了
		}
		Draw();
		SDL_Delay(16);
	}
	End();
}

void Graphic::End() const {
	//ウィンドウを閉じる
	for (auto& window : windows)
		window.second.Destroy();
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

void Graphic::CreateScreen(int id, const string& title, int width, int height, int mode) {
	if (windows.count(id)) {
		windows[id].SetTitle(title);
		windows[id].Resize(width, height);
		windows[id].SetFullscreen(mode & 1);
		windows[id].MakeCurrent();
		windows[id].MakeTop();
		WinID = id; // 既存のウィンドウを更新
		return; // 既存のウィンドウを更新
	}
	windows[id].Create(false, title, width, height, mode);
	WinID = id; // 新しいウィンドウを作成したので、現在のウィンドウIDを更新

	windows[WinID].font.Init(characters, glyph_atlas, &windows[WinID].projection);
	windows[WinID].font.SetFont("C:\\Windows\\Fonts\\msgothic.ttc", font_size);
	windows[WinID].shape.Init(width, height, &windows[WinID].projection);
	windows[WinID].image.Init(images, &windows[WinID].projection);
}

void Graphic::MakeCurrentWindow(int id, int mode) {
	Draw(true); // 強制的に描画を更新
	if (windows.count(id)) {
		WinID = id; // 現在のウィンドウIDを更新
		windows[WinID].MakeCurrent();
		if (mode) windows[WinID].MakeTop();
	} else {
		throw WindowException("Window ID does not exist.");
	}
}

void Graphic::HideWindow(int id) {
	if (windows.count(id)) {
		windows[id].Hide();
	} else {
		throw WindowException("Window ID does not exist.");
	}
}

void Graphic::ShowWindow(int id) {
	if (windows.count(id)) {
		windows[id].Show();
	} else {
		throw WindowException("Window ID does not exist.");
	}
}

void Graphic::DestroyWindow(int id) {
	if (WinID == id)
		throw WindowException("Cannot destroy the current window. Please switch to another window first.");
	if (windows.count(id)) {
		windows[id].Destroy();
		windows.erase(id); // ウィンドウを削除
	} else {
		throw WindowException("Window ID does not exist.");
	}
}

void Graphic::ResizeWindow(int new_width, int new_height) {
	if (new_width <= 0 || new_height <= 0) return; // 無効なサイズは無視
	windows[WinID].Resize(new_width, new_height);
}

void Graphic::SetWindowTitle(const std::string& title) const {
	windows.at(WinID).SetTitle(title);
}

void Graphic::SetWindowPosition(int id, int x, int y) {
	if (windows.count(id)) {
		windows[id].SetPosition(x, y);
	} else {
		throw WindowException("Window ID does not exist.");
	}
}

void Graphic::Gcopy(int id, int src_x, int src_y, int src_width, int src_height, int dst_x, int dst_y, int dst_width, int dst_height) {
	if (windows.count(id)) {
		if (id == WinID)return;
		if (src_width < 0)src_width = windows[id].Width(); // デフォルトはウィンドウの幅
		if (src_height < 0)src_height = windows[id].Height(); // デフォルトはウィンドウの高さ
		if (dst_width < 0) dst_width = windows[WinID].Width(); // デフォルトは現在のウィンドウの幅
		if (dst_height < 0) dst_height = windows[WinID].Height(); // デフォルトは現在のウィンドウの高さ
		src_y = windows[id].Height() - src_y - src_height; // SDLの座標系に合わせてY座標を調整
		dst_y = windows[WinID].Height() - dst_y - dst_height;
		AllVertexData new_data;
		new_data.all_vertices.push_back((float)src_x);
		new_data.all_vertices.push_back((float)src_y + src_height);
		new_data.all_vertices.push_back((float)src_x + src_width);
		new_data.all_vertices.push_back((float)src_y);
		new_data.all_vertices.push_back((float)dst_x);
		new_data.all_vertices.push_back((float)dst_y + dst_height);
		new_data.all_vertices.push_back((float)dst_x + dst_width);
		new_data.all_vertices.push_back((float)dst_y);
		new_data.division = id;
		new_data.gmode = 0; // 通常の描画モード
		new_data.ID = 6;
		windows[WinID].all_vertices.push_back(new_data);
	} else {
		throw WindowException("Window ID does not exist.");
	}
}