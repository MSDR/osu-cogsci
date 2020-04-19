#ifndef HITCIRCLE_H_
#define HITCIRCLE_H_

#include "Globals.h"
#include "graphics.h"
#include "Sprite.h"

class HitCircle {
public:
	HitCircle(Sprite* hitCircle, Sprite* hitCircleOverlay, Sprite* approachCircle, int offset, Vector2 coords, int comboNum) 
		: hitCircle_(hitCircle), hitCircleOverlay_(hitCircleOverlay), approachCircle_(approachCircle), 
		  offset_(offset), coords_(coords), comboNum_(comboNum), clicked_(false) {};

	~HitCircle();

	void draw(Graphics &graphics, int currentOffset);

	int getOffset() { return offset_; }
	Vector2 getCoords() { return coords_; }
	bool clicked_;
private:
	int offset_;
	Vector2 coords_;
	int comboNum_;


	//References to hitCircle visuals
	Sprite* hitCircle_;
	Sprite* hitCircleOverlay_;
	Sprite* approachCircle_;
};

#endif