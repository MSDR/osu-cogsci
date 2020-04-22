#include "HitCircle.h"

HitCircle::~HitCircle() {
}

void HitCircle::draw(Graphics &graphics, int currentOffset) {
	int timingDiff = offset_ - currentOffset;
	if (timingDiff > APPROACH_CIRCLE_RATE || timingDiff < -APPROACH_CIRCLE_RATE || clicked_) { //circle's been clicked or it's too early to draw
		return;
	} else { //draw circle
		int drawX = (coords_.x - HIT_CIRCLE_RADIUS);
		int drawY = (coords_.y - HIT_CIRCLE_RADIUS);
		hitCircle_->draw(graphics, drawX, drawY, true);
		hitCircleOverlay_->draw(graphics, drawX, drawY, true);

		float acScale = 1.0 + (timingDiff / ((float)APPROACH_CIRCLE_RATE));
		int acCoordsOffset = (acScale-1)*HIT_CIRCLE_RADIUS+HIT_CIRCLE_RADIUS;

		if(timingDiff > 0) 
			approachCircle_->draw(graphics, coords_.x - acCoordsOffset, coords_.y - acCoordsOffset, true, acScale, acScale);
	}
}
