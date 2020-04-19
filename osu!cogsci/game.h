#ifndef GAME_H_
#define GAME_H_

#include <algorithm>?
#include <cmath>
#include <iostream>
#include <vector>

#include "SDL_include/SDL_mixer.h"

#include "Graphics.h"
#include "HitCircle.h"
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

	int msCounter_;

	int beatmap_;

	HitCircle* testCircle_;
	HitCircle* testCircle2_;

	std::vector<HitCircle*> hitCircles_;
	Sprite* hitCircle_;
	Sprite* hitCircleOverlay_;
	Sprite* approachCircle_; //remember to delete and re-create these with each change in circle radius

	Mix_Chunk* normalHitNormal_;
};

#endif GAME_H_