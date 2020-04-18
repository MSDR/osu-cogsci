#ifndef GAME_H_
#define GAME_H_

#include <algorithm>?
#include <iostream>

#include "SDL_include/SDL_mixer.h"

#include "Graphics.h"
#include "Input.h"
#include "Sprite.h"
#include "Text.h"

class Game {
public:
	Game();
	~Game();

private:
	void gameLoop();

	void draw(Graphics &graphics);

	void update(float elapsedTime);

	Sprite sprite_;

	int msCounter_;

	int beatmap_;

	SDL_Surface** hitCircleParts;
};

#endif GAME_H_