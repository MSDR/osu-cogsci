#include "HitCircle.h"

HitCircle::~HitCircle() {
}

void HitCircle::draw(Graphics &graphics, int currentOffset, Sprite* numSprite[9]) {
	int timingDiff = offset_ - currentOffset;
	if (timingDiff > APPROACH_CIRCLE_RATE || timingDiff < -APPROACH_CIRCLE_RATE || clicked_) { //circle's been clicked or it's too early to draw
		return;
	} else { //draw circle
		double oCoefficient = 255.0 / std::pow(APPROACH_CIRCLE_RATE, 3.0);
		int opacity = timingDiff < 0 ? 0 : 255.0 - oCoefficient*std::pow(timingDiff, 3.0);
		//std::cout << oCoefficient << " " << opacity << " " << timingDiff<< std::endl;

		int drawX = (coords_.x - HIT_CIRCLE_RADIUS);
		int drawY = (coords_.y - HIT_CIRCLE_RADIUS);
		hitCircle_->draw(graphics, drawX, drawY, true, 1.0, 1.0, opacity);
		hitCircleOverlay_->draw(graphics, drawX, drawY, true, 1.0, 1.0, opacity);
		std::cout << drawX << " " << drawY << " AR " << APPROACH_CIRCLE_RATE << " CS " << HIT_CIRCLE_RADIUS << std::endl;

		//Draw numbers in the circles
		//Only handles numbers up to 99, but I don't think Osu! allows anything past that either
		if (comboNum_ < 10) {
			int numX = coords_.x - HIT_CIRCLE_RADIUS / 3;
			int numY = coords_.y - HIT_CIRCLE_RADIUS / 3;
			numSprite[comboNum_]->draw(graphics, numX, numY, true, 1.0, 1.0, opacity);
		}
		else {
			int numX = coords_.x - HIT_CIRCLE_RADIUS / 2;
			int numY = coords_.x - HIT_CIRCLE_RADIUS / 3;
			std::string comboStr = std::to_string(comboNum_);
			numSprite[std::stoi(comboStr[0]+"")]->draw(graphics, numX, numY, true, 1.0, 1.0, opacity);
			numSprite[std::stoi(comboStr[1]+"")]->draw(graphics, numX+45, numY, true, 1.0, 1.0, opacity);
		}

		float acScale = 1.0 + (timingDiff / ((float)APPROACH_CIRCLE_RATE));
		int acCoordsOffset = (acScale-1)*HIT_CIRCLE_RADIUS+HIT_CIRCLE_RADIUS;

		if(timingDiff > 0) 
			approachCircle_->draw(graphics, coords_.x - acCoordsOffset, coords_.y - acCoordsOffset, true, acScale, acScale, 255);
	}
}

void HitCircle::setCombo(int num) {
	comboNum_ = num;
}

int HitCircle::getCombo() const {
	return comboNum_;
}

int HitCircle::getOffset() const {
	return offset_;
}