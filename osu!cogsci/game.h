#ifndef GAME_H_
#define GAME_H_

#include <algorithm>?
#include <cmath>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>

#include "SDL_include/SDL_mixer.h"

#include "HitCircle.h"
#include "Input.h"
#include "Text.h"


class Game {
public:
	Game();

private:
	void gameLoop();

	void draw(Graphics &graphics);

	void update(float elapsedTime);

	int msCounter_;
	int beatmap_;
	int num300_, num100_, num50_, numMiss_, notesPassed_;

	float CS_;
	float AR_;
	float accuracy_;
	float ms300_, ms100_, ms50_;

	HitCircle* testCircle_;
	HitCircle* testCircle2_;

	std::vector<HitCircle*> hitCircles_;
	Sprite* numSprite_[10];
	Sprite* scoreSprite_[12]; //0-9 are numbers, 10 is ".", 11 is "%"
	Sprite* sprite300_;
	Sprite* sprite100_;
	Sprite* sprite50_;
	Sprite* sprite0_;
	Sprite* hitCircle_;
	Sprite* hitCircleOverlay_;
	Sprite* approachCircle_; //remember to delete and re-create these with each change in circle radius

	Mix_Chunk* normalHitNormal_;

	std::vector <std::pair<Sprite*, int>> hitSprites_;
	std::vector <Vector2> hitSpriteCoords_;
};

#endif GAME_H_