//Sprite class adapted from LimeOats: https://limeoats.com/, modified and commented by Mason Sklar [2020]

#ifndef SPRITE_H_
#define SPRITE_H_

#include <SDL.h>
#include <string>

#include "Globals.h"
#include "Graphics.h"

class Sprite {
public:
	Sprite();
	Sprite(Graphics &graphics, const std::string &filePath, int sourceX, int sourceY, int width, int height, float posX=0, float posY=0);
	virtual ~Sprite();

	//Updates the Sprite and its members
	virtual void update(float elapsedTime);

	//Draws the Sprite onto the screen
	void draw(Graphics &graphics, int x, int y, bool nullSRect = false, float hScale = 1.0, float wScale = 1.0);

	const float getWidth() const;
	const float getHeight() const;
	const float getX() const;
	const float getY() const;

protected:
	SDL_Rect sourceRect_;
	SDL_Texture* spriteSheet_;
	float x_;
	float y_;

};

#endif SPRITE_H_