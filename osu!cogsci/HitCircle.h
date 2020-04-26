#ifndef HITCIRCLE_H_
#define HITCIRCLE_H_

#include "Sprite.h"

class HitCircle {
public:
	HitCircle(Sprite* hitCircle, Sprite* hitCircleOverlay, Sprite* approachCircle, int offset, Vector2 coords, int comboNum) 
		: hitCircle_(hitCircle), hitCircleOverlay_(hitCircleOverlay), approachCircle_(approachCircle), 
		offset_(offset), coords_(coords), comboNum_(comboNum), clicked_(false)
	{
		//adjust coords_ from osu's 512x384 grid to account for display resolution
		coords_.x = coords_.x*COORDINATE_SCALE + (SCREEN_WIDTH-512 * COORDINATE_SCALE)/2;
		coords_.y = coords_.y*COORDINATE_SCALE + (SCREEN_HEIGHT-384 * COORDINATE_SCALE)/2;
	};

	~HitCircle();

	void draw(Graphics &graphics, int currentOffset, Sprite* numSprite[9]);
	void setCombo(int num);

	int getCombo() const;
	int getOffset() const;
	int getOffset() { return offset_; }
	Vector2 getCoords() { return coords_; }
	bool clicked_;

private:
	Vector2 coords_; //Holds the position of the center of a circle relative to the screen
	int offset_;
	int comboNum_;

	//References to hitCircle visuals
	Sprite* hitCircle_;
	Sprite* hitCircleOverlay_;
	Sprite* approachCircle_;
};

#endif