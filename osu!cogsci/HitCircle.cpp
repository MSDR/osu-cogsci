#include "HitCircle.h"

#include <iostream>
HitCircle::~HitCircle() {
}

void HitCircle::draw(Graphics & graphics, int currentOffset) {
	int timingDiff = offset_ - currentOffset;
	if (timingDiff > globals::APPROACH_CIRCLE_RATE && !clicked_) {
		return;
	} else {
		std::cout << "successful draw" << std::endl;
		hitCircle_->draw(graphics, coords_.x, coords_.y, true);
		hitCircleOverlay_->draw(graphics, coords_.x, coords_.y, true);
		float acScale = 1.0 + (timingDiff) / ((float)globals::APPROACH_CIRCLE_RATE);
		if(timingDiff > 0) approachCircle_->draw(graphics, coords_.x + 0.5*(1.0-acScale)*globals::HIT_CIRCLE_RADIUS, coords_.y + 0.5*(1.0-acScale)*globals::HIT_CIRCLE_RADIUS, true, acScale, acScale);
	}
}
