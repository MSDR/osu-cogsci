#include "HitCircle.h"

#include <iostream>
HitCircle::~HitCircle() {
}

void HitCircle::draw(Graphics &graphics, int currentOffset) {
	int timingDiff = offset_ - currentOffset;
	if (timingDiff > globals::APPROACH_CIRCLE_RATE || timingDiff < -globals::APPROACH_CIRCLE_RATE || clicked_) {
		return;
	} else {
		hitCircle_->draw(graphics, coords_.x - globals::HIT_CIRCLE_RADIUS, coords_.y - globals::HIT_CIRCLE_RADIUS, true);
		hitCircleOverlay_->draw(graphics, coords_.x - globals::HIT_CIRCLE_RADIUS, coords_.y - globals::HIT_CIRCLE_RADIUS, true);
		float acScale = 1.0 + (timingDiff / ((float)globals::APPROACH_CIRCLE_RATE));
		int acCoordsOffset = (acScale-1)*globals::HIT_CIRCLE_RADIUS+globals::HIT_CIRCLE_RADIUS;
		if(timingDiff > 0) 
			approachCircle_->draw(graphics, coords_.x - acCoordsOffset, coords_.y - acCoordsOffset, true, acScale, acScale);
	}
}
