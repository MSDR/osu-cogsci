#ifndef GAME_H_
#define GAME_H_

#include <algorithm>?
#include <cmath>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include <cmath>

#include "SDL_include/SDL_mixer.h"

#include "HitCircle.h"
#include "Input.h"
#include "Text.h"


class Game {
public:
	Game();

private:
	void gameLoop();

	void fillCircleVec(const std::string& fileName, Sprite* circleSprite, Sprite* circleOverlay, Sprite* approachCircle, Graphics& graphics);

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

	std::vector <std::pair<Sprite*, int> > hitSprites_;
	std::vector <Vector2> hitSpriteCoords_;

	/*
	std::vector<Sprite*> lineSprites_;
	std::vector<std::pair<int, int> > lineOffsets_;
	std::vector<Vector2> lineSpriteCoords_;
	std::vector<SDL_Point*> lineCenters_;
	std::vector<float> lineAngles_;
	*/

	//Vectors for data collection

	//The size of these will be hitCirles_.size()-2
	//Index 0 will have distance between circle 0 and 1, index 1 will be between 1 and 2, etc.
	std::vector<float> distBetween_;
	std::vector<int> timeBetween_;

	//The size of these will be hitCircle_.size()
	std::vector<float> distFromCenter_;
	std::vector<int> timeOffPerfect_;
};

#endif GAME_H_