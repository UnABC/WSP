#include "graphic.hpp"
using namespace std;

Graphic::Graphic(int width, int height, bool is_fullscreen) : width(width), height(height), is_fullscreen(is_fullscreen) {
	//色々初期化
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
		throw WindowException("Failed to initialize SDL.");
	//if (TTF_Init() < 0)FailedToInitialize("Failed to initialize SDL_ttf.");
	//ウィンドウ作成
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 5);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	window = SDL_CreateWindow("WSP", 640, 480, (is_fullscreen ? SDL_WINDOW_FULLSCREEN : SDL_WINDOW_HIDDEN) | SDL_WINDOW_OPENGL);
	if (!window)FailedToInitialize("Failed to create SDL Window.");
	//レンダラー作成
	// renderer = SDL_CreateRenderer(window, NULL);
	// if (!renderer)FailedToInitialize("Failed to create SDL Renderer.");
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
	//フォント初期化+読み込み
	font.Init(width, height);
	font.SetFont("C:\\Windows\\Fonts\\msgothic.ttc", font_size);
	// FT_Init_FreeType(&library);
	// FT_New_Face(library, "C:\\Windows\\Fonts\\msgothic.ttc", 0, &face);
	// FT_Set_Pixel_Sizes(face, 0, 16);
	//font = TTF_OpenFont("C:\\Windows\\Fonts\\msgothic.ttc", 16);
	//if (!font)FailedToInitialize("Failed to load font.");
	//ウィンドウ表示
	SDL_ShowWindow(window);
}

void Graphic::FailedToInitialize(const string& message) {
	End();
	throw WindowException(message);
}
/*
SDL_Texture* Graphic::CashText(const string& text) {
	//キャッシュにあるか確認
	if (text_cache.count(text))
		return text_cache[text];

	//キャッシュにない場合は新しく作成
	SDL_Surface* textSurface = TTF_RenderText_Blended(font, text.c_str(), text.size(), color);
	if (!textSurface)FailedToInitialize("Failed to create text surface.");
	SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
	SDL_DestroySurface(textSurface);
	if (!textTexture)FailedToInitialize("Failed to create text texture from surface.");
	text_cache[text] = textTexture;
	return textTexture;
}*/

void Graphic::printText(const string& text) {
	if (!SDL_GL_SwapWindow(window))
		FailedToInitialize(SDL_GetError());
	font.SetTexts(text, pos.x, pos.y, 1.0f, color, width);
	if (!SDL_GL_SwapWindow(window))
		FailedToInitialize(SDL_GetError());
	// SDL_Texture* textTexture = CashText(text);
	// float texW, texH;
	// SDL_GetTextureSize(textTexture, &texW, &texH);
	// SDL_FRect FromRect = { 0.0, 0.0, texW, texH };
	// SDL_FRect ToRect = { pos.x, pos.y, texW, texH };
	//下にずらす
	pos.y += font_size;
	// if (redraw) {
	// 	if (SDL_RenderTexture(renderer, textTexture, &FromRect, &ToRect)) {
	// 		throw WindowException(SDL_GetError());
	// 	}
	// 	SDL_DestroyTexture(textTexture);
	// 	SDL_RenderPresent(renderer);
	// } else {
	// 	textures.push_back(make_tuple(textTexture, FromRect, ToRect));
	// }
}

void Graphic::Clear() {
	//テクスチャのクリア
	for (const auto& texture : textures)
		SDL_DestroyTexture(get<0>(texture));
	textures.clear();
	/*SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
	SDL_RenderClear(renderer);
	SDL_RenderPresent(renderer);*/
	pos = { 0, 0 };	//表示位置を初期化
	color = { 255, 255, 255, 255 };	//色を初期化
}

void Graphic::Draw() {
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	//テクスチャの描画
	font.DrawTexts();
	/*for (const auto& texture : textures) {
		SDL_RenderTexture(renderer, get<0>(texture), &get<1>(texture), &get<2>(texture));
		SDL_DestroyTexture(get<0>(texture));
	}
	textures.clear();
	SDL_RenderPresent(renderer);*/
	if (!SDL_GL_SwapWindow(window))
		FailedToInitialize(SDL_GetError());
}

void Graphic::Stop() {
	bool running = true;
	SDL_Event event;
	while (running) {
		while (SDL_PollEvent(&event)) if (event.type == SDL_EVENT_QUIT) running = false;
		Draw();
		//背景の初期化
		// glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		//glClear(GL_COLOR_BUFFER_BIT);

		//printText("Hello World");

		//テクスチャの描画
		//SDL_GL_SwapWindow(window);
	}
	End();
}

void Graphic::End() const {
	//ウィンドウを閉じる
	//if (face)FT_Done_Face(face);
	//if (library)FT_Done_FreeType(library);
	SDL_GL_DestroyContext(glContext);
	SDL_DestroyWindow(window);
	//SDL_DestroyRenderer(renderer);
	SDL_Quit();
	//TTF_Quit();
}

