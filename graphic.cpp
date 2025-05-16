#include "graphic.hpp"

Graphic::Graphic(int width, int height, bool is_fullscreen) : width(width), height(height), is_fullscreen(is_fullscreen) {
	//色々初期化
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
		throw WindowException("Failed to initialize SDL.");
	if (TTF_Init() < 0)FailedToInitialize("Failed to initialize SDL_ttf.");
	//ウィンドウ作成
	window = SDL_CreateWindow("WSP", 640, 480, is_fullscreen ? SDL_WINDOW_FULLSCREEN : SDL_WINDOW_HIDDEN);
	if (!window)FailedToInitialize("Failed to create SDL Window.");
	//レンダラー作成
	renderer = SDL_CreateRenderer(window, NULL);
	if (!renderer)FailedToInitialize("Failed to create SDL Renderer.");
	//フォント読み込み
	font = TTF_OpenFont("C:\\Windows\\Fonts\\msgothic.ttc", 16);
	if (!font)FailedToInitialize("Failed to load font.");
	//ウィンドウ表示
	SDL_ShowWindow(window);
}

void Graphic::FailedToInitialize(const std::string& message) {
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
	TTF_Quit();
	throw WindowException(message);
}

void Graphic::printText(const std::string& text) {
	SDL_Surface* textSurface = TTF_RenderText_Blended(font, text.c_str(), text.size(), color);
	if (!textSurface)FailedToInitialize("Failed to create text surface.");
	SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
	SDL_DestroySurface(textSurface);
	if (!textTexture)FailedToInitialize("Failed to create text texture from surface.");
	float texW, texH;
	SDL_GetTextureSize(textTexture, &texW, &texH);
	SDL_FRect FromRect = { 0.0, 0.0, texW, texH };
	SDL_FRect ToRect = { pos.x, pos.y, texW, texH };
	//下にずらす
	pos.y += texH;
	textures.push_back(std::make_tuple(textTexture,FromRect,ToRect));
	Draw();
}

void Graphic::Draw() const {
	//ウィンドウの背景色を設定
	//SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
	//テクスチャの描画
	for (const auto& texture : textures)
		SDL_RenderTexture(renderer, get<0>(texture), &get<1>(texture), &get<2>(texture));

	SDL_RenderPresent(renderer);
}

void Graphic::Stop() {
	bool running = true;
	while (running) {
		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_EVENT_QUIT) running = false;
		}
		Draw();
		SDL_Delay(16);
	}
	SDL_DestroyWindow(window);
	SDL_DestroyRenderer(renderer);
	SDL_Quit();
}


