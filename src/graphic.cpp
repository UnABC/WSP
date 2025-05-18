#include "graphic.hpp"

Graphic::Graphic(int width, int height, bool is_fullscreen) : width(width), height(height), is_fullscreen(is_fullscreen) {
	//色々初期化
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
		throw WindowException("Failed to initialize SDL.");
	//if (TTF_Init() < 0)FailedToInitialize("Failed to initialize SDL_ttf.");
	//ウィンドウ作成
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 5);
	window = SDL_CreateWindow("WSP", 640, 480, (is_fullscreen ? SDL_WINDOW_FULLSCREEN : SDL_WINDOW_HIDDEN) | SDL_WINDOW_OPENGL);
	if (!window)FailedToInitialize("Failed to create SDL Window.");
	//レンダラー作成
	renderer = SDL_CreateRenderer(window, NULL);
	if (!renderer)FailedToInitialize("Failed to create SDL Renderer.");
	//フォント読み込み
	FT_Init_FreeType(&library);
	FT_New_Face(library, "C:\\Windows\\Fonts\\msgothic.ttc", 0, &face);
	FT_Set_Pixel_Sizes(face, 0, 16);
	//font = TTF_OpenFont("C:\\Windows\\Fonts\\msgothic.ttc", 16);
	//if (!font)FailedToInitialize("Failed to load font.");
	//OpenGLの初期化
	glContext = SDL_GL_CreateContext(window);
	if (!glContext)FailedToInitialize("Failed to create OpenGL context.");
	//OpenGLの有効化
	if (SDL_GL_MakeCurrent(window, glContext) < 0)
		FailedToInitialize("Failed to make OpenGL context current.");
	glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	SDL_GL_SwapWindow(window);
	//ウィンドウ表示
	SDL_ShowWindow(window);
}

void Graphic::FailedToInitialize(const std::string& message) {
	End();
	throw WindowException(message);
}

SDL_Texture* Graphic::CashText(const std::string& text) {
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
}

void Graphic::printText(const std::string& text) {
	SDL_Texture* textTexture = CashText(text);
	float texW, texH;
	SDL_GetTextureSize(textTexture, &texW, &texH);
	SDL_FRect FromRect = { 0.0, 0.0, texW, texH };
	SDL_FRect ToRect = { pos.x, pos.y, texW, texH };
	//下にずらす
	pos.y += texH;
	if (redraw){
		if (SDL_RenderTexture(renderer, textTexture, &FromRect, &ToRect)){
			throw WindowException(SDL_GetError());
		}
		SDL_DestroyTexture(textTexture);
		SDL_RenderPresent(renderer);
	}else{
		textures.push_back(std::make_tuple(textTexture,FromRect,ToRect));
	}
}

void Graphic::Clear() {
	//テクスチャのクリア
	for (const auto& texture : textures)
		SDL_DestroyTexture(get<0>(texture));
	textures.clear();
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_RenderClear(renderer);
	SDL_RenderPresent(renderer);
	pos = { 0, 0 };	//表示位置を初期化
	color = { 255, 255, 255, 255 };	//色を初期化
}

void Graphic::Draw() {
	//テクスチャの描画
	for (const auto& texture : textures){
		SDL_RenderTexture(renderer, get<0>(texture), &get<1>(texture), &get<2>(texture));
		SDL_DestroyTexture(get<0>(texture));
	}
	textures.clear();
	SDL_RenderPresent(renderer);
}

void Graphic::Stop() {
	bool running = true;
	while (running) {
		SDL_Event event;
		while (SDL_PollEvent(&event)) if (event.type == SDL_EVENT_QUIT) running = false;

		//背景の初期化
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		//テクスチャの描画
		SDL_GL_SwapWindow(window);
	}
	End();
}

void Graphic::End() const {
	//ウィンドウを閉じる
	FT_Done_Face(face);
	FT_Done_FreeType(library);
	SDL_GL_DestroyContext(glContext);
	SDL_DestroyWindow(window);
	SDL_DestroyRenderer(renderer);
	SDL_Quit();
	TTF_Quit();
}

