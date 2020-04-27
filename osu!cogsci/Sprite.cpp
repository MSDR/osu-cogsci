//Sprite class adapted from LimeOats: https://limeoats.com/, modified and commented by Mason Sklar [2019]

#include "Sprite.h"

Sprite::Sprite() {
}

Sprite::Sprite(Graphics &graphics, const std::string &filePath, int sourceX, int sourceY, int width, int height, float posX, float posY) :
	x_(posX),
	y_(posY)
{
	sourceRect_.x = sourceX;
	sourceRect_.y = sourceY;
	sourceRect_.w = width;
	sourceRect_.h = height;

	spriteSheet_ = SDL_CreateTextureFromSurface(graphics.getRenderer(), graphics.loadImage(filePath));

	if (spriteSheet_ == NULL) {
		std::cerr << "\nError: Unable to load image at filepath " << filePath << "\n";
	}

}

Sprite::~Sprite() {
}

void Sprite::draw(Graphics &graphics, int x, int y, bool nullSRect, float hScale, float wScale, float opacity) {
	SDL_SetTextureAlphaMod(spriteSheet_, opacity);
	SDL_Rect destinationRectangle = { x, y, sourceRect_.w * hScale, sourceRect_.h * wScale };
	graphics.blitSurface(spriteSheet_, nullSRect ? NULL : &sourceRect_, &destinationRectangle);
}

void Sprite::update(float elapsedTime) {
}

const float Sprite::getWidth() const {
	return sourceRect_.w;
}

const float Sprite::getHeight() const {
	return sourceRect_.h;
}

const float Sprite::getX() const {
	return x_;
}

const float Sprite::getY() const {
	return y_;
}