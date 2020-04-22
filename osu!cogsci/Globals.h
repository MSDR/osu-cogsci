//Globals class adapted from LimeOats: https://limeoats.com/, modified and commented by Mason Sklar [2020]

#ifndef  GLOBALS_H_
#define GLOBALS_H_

//defined in graphics.cpp
extern float COORDINATE_SCALE;
extern int SCREEN_WIDTH;
extern int SCREEN_HEIGHT;
extern int APPROACH_CIRCLE_RATE;
extern int HIT_CIRCLE_RADIUS;


namespace sides {
	enum Side {
		TOP,
		BOTTOM,
		LEFT,
		RIGHT,
		NONE
	};

	inline Side getOppositeSide(Side side) {
		return
			side == TOP ? BOTTOM :
			side == BOTTOM ? TOP :
			side == LEFT ? RIGHT :
			side == RIGHT ? LEFT :
			NONE;
	}
}

enum Direction {
	LEFT,
	RIGHT,
	UP,
	DOWN
};

struct Vector2f {
	float x, y;
	Vector2f() : x(0), y(0) {}
	Vector2f(float x, float y) : x(x), y(y) {}

	Vector2f zero() {
		return Vector2f(0, 0);
	}
};

struct Vector2 {
	int x, y;
	Vector2() : x(0), y(0) {}
	Vector2(int x, int y) : x(x), y(y) {}

	Vector2 zero() {
		return Vector2(0, 0);
	}
};

#endif GLOBALS_H_