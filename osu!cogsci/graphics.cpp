//Graphics class adapted from LimeOats: https://limeoats.com/, modified and commented by Mason Sklar [2020]

#include "graphics.h"

int SCREEN_WIDTH;
int SCREEN_HEIGHT;
float COORDINATE_SCALE;

Graphics::Graphics() {
	//find display resolution and assign global variables accordingly
	SDL_Rect* r = new SDL_Rect();
	SDL_GetDisplayBounds(0, r);
	SCREEN_WIDTH = r->w;
	SCREEN_HEIGHT =  r->h;
	COORDINATE_SCALE = SCREEN_HEIGHT / 512;

	//create window
	SDL_CreateWindowAndRenderer(SCREEN_WIDTH, SCREEN_HEIGHT, 0, &window_, &renderer_);
	SDL_SetWindowTitle(window_, "osu!cogsci");
	SDL_SetWindowFullscreen(window_, SDL_TRUE);
}

Graphics::~Graphics() {
	SDL_DestroyWindow(window_);
	SDL_DestroyRenderer(renderer_);
}

SDL_Surface* Graphics::loadImage(const std::string &filePath) {
	if (spriteSheets_.count(filePath) == 0) {
		spriteSheets_[filePath] = IMG_Load(filePath.c_str());
	}

	return spriteSheets_[filePath];
}

void Graphics::blitSurface(SDL_Texture* texture, SDL_Rect* sourceRectangle, SDL_Rect* destinationRectangle) {
	SDL_RenderCopy(renderer_, texture, sourceRectangle, destinationRectangle);
}

void Graphics::flip() {
	SDL_RenderPresent(renderer_);
}

void Graphics::clear() {
	SDL_RenderClear(renderer_);
}

SDL_Renderer* Graphics::getRenderer() const {
	return renderer_;
}