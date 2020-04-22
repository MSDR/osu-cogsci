//Graphics class adapted from LimeOats: https://limeoats.com/, modified and commented by Mason Sklar [2020]

#include "graphics.h"

int SCREEN_WIDTH;
int SCREEN_HEIGHT;
float COORDINATE_SCALE;
int APPROACH_CIRCLE_RATE;
int HIT_CIRCLE_RADIUS;

Graphics::Graphics(float CS, float AR) {
	//Find display resolution and assign global variables accordingly
	SDL_Rect* r = new SDL_Rect();
	SDL_GetDisplayBounds(0, r);
	SCREEN_WIDTH = r->w;
	SCREEN_HEIGHT =  r->h;
	COORDINATE_SCALE = SCREEN_HEIGHT / 512;

	//Create window
	SDL_CreateWindowAndRenderer(SCREEN_WIDTH, SCREEN_HEIGHT, 0, &window_, &renderer_);
	SDL_SetWindowTitle(window_, "osu!cogsci");
	SDL_SetWindowFullscreen(window_, SDL_TRUE);

	//Assign Circle Size and Approach Rate
	HIT_CIRCLE_RADIUS = COORDINATE_SCALE*((109 - 9 * CS)/2.0);
	std::cout << "CS: " << CS << " HIT_RADIUS: " << HIT_CIRCLE_RADIUS << std::endl;
	if (AR < 4.95) APPROACH_CIRCLE_RATE = 1200 + 600 * (5 - AR) / 5;
	else if (AR > 4.95) APPROACH_CIRCLE_RATE = 1200 - 750 * (AR - 5) / 5;
	else APPROACH_CIRCLE_RATE = 1200;
	std::cout << "AR: " << AR << " APPROACH_RATE: " << APPROACH_CIRCLE_RATE << std::endl;
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