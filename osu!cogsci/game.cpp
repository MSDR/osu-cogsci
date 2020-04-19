#include "Game.h"

namespace {
	const int FPS = 60;
	const int MAX_FRAME_TIME = 75;
}

namespace input_keys {
	SDL_Scancode key_name = SDL_SCANCODE_0;
}

Game::Game() {
	SDL_Init(SDL_INIT_EVERYTHING);
	IMG_Init(IMG_INIT_PNG);
	Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);
	msCounter_ = -10000;
	beatmap_ = -1;
	gameLoop();

	//Close SDL
	IMG_Quit();
	SDL_Quit();
}

Game::~Game() {

}

void Game::gameLoop() {
	Graphics graphics;
	Input input;
	SDL_Event event;

	normalHitNormal_ = Mix_LoadWAV("Skin/normal-hitnormal.wav");

	hitCircle_ = new Sprite(graphics, "Skin/hitcircle@2x.png", 0, 0, globals::HIT_CIRCLE_RADIUS*2, globals::HIT_CIRCLE_RADIUS*2);
	hitCircleOverlay_ = new Sprite(graphics, "Skin/hitcircleoverlay@2x.png", 0, 0, globals::HIT_CIRCLE_RADIUS*2, globals::HIT_CIRCLE_RADIUS*2);
	approachCircle_ = new Sprite(graphics, "Skin/approachcircle@2x.png", 0, 0, globals::HIT_CIRCLE_RADIUS*2, globals::HIT_CIRCLE_RADIUS*2);

	hitCircles_.push_back(new HitCircle(hitCircle_, hitCircleOverlay_, approachCircle_, 2000, Vector2(500, 500), 1));
	hitCircles_.push_back(new HitCircle(hitCircle_, hitCircleOverlay_, approachCircle_, 3000, Vector2(700, 700), 2));
	hitCircles_.push_back(new HitCircle(hitCircle_, hitCircleOverlay_, approachCircle_, 4000, Vector2(900, 500), 2));

	int LAST_UPDATE_TIME = SDL_GetTicks();

	//Load custom cursor
	SDL_Surface* cursorSurface = IMG_Load("Skin/cursor.png");
	if (cursorSurface) {
		SDL_Cursor* cursor = SDL_CreateColorCursor(cursorSurface, 0, 0);
		if (cursor) SDL_SetCursor(cursor);
	}

	//Start game loop
	while (true) {
		input.beginNewFrame();
		if (SDL_PollEvent(&event)) {
			if (event.type == SDL_KEYDOWN) {
				if (event.key.repeat == 0) {
					input.keyDownEvent(event);
				}
			} else if (event.type = SDL_KEYUP) {
				input.keyUpEvent(event);
			} else if (event.type == SDL_QUIT) {
				return;
			}
		}

		//Handle inputs
		if (input.wasKeyPressed(SDL_SCANCODE_DELETE)) {
			return;
		}

		if (input.wasKeyPressed(SDL_SCANCODE_X) || input.wasKeyPressed(SDL_SCANCODE_Z)) {
			for (int i = 0; i < hitCircles_.size(); ++i) {
				if (hitCircles_[i]->getOffset() - msCounter_ >= globals::APPROACH_CIRCLE_RATE) {
					break;
				}
				int mouseX = 0;
				int mouseY = 0;
				SDL_GetMouseState(&mouseX, &mouseY);
				mouseX += cursorSurface->w / 2;
				mouseY += cursorSurface->h / 2;
				std::cout << std::sqrt(std::pow(hitCircles_[i]->getCoords().x - mouseX, 2) + std::pow(hitCircles_[i]->getCoords().y - mouseY, 2)) << std::endl;
				if (std::sqrt(std::pow(hitCircles_[i]->getCoords().x - mouseX, 2) + std::pow(hitCircles_[i]->getCoords().y - mouseY, 2)) <= globals::HIT_CIRCLE_RADIUS) {
					hitCircles_[i]->clicked_ = true;
					Mix_PlayChannel(-1, normalHitNormal_, 0);
					break;
				}
			}
		
		}

		//Update timing
		const int CURRENT_TIME_MILLIS = SDL_GetTicks();
		int ELAPSED_TIME_MILLIS = CURRENT_TIME_MILLIS - LAST_UPDATE_TIME;
		msCounter_ += ELAPSED_TIME_MILLIS;
		update(std::min(ELAPSED_TIME_MILLIS, MAX_FRAME_TIME));
		LAST_UPDATE_TIME = CURRENT_TIME_MILLIS;
		draw(graphics);
		//std::cout << msCounter_ << std::endl;
	}
}

void Game::draw(Graphics &graphics) {
	graphics.clear();
	std::vector<HitCircle*>::iterator itr = hitCircles_.end();
	--itr;
	for(int i = hitCircles_.size()-1; i >=0 ; --i) {
		if (hitCircles_[i]->getOffset() - msCounter_ < -globals::APPROACH_CIRCLE_RATE || hitCircles_[i]->clicked_) {
			delete hitCircles_[i];
			itr = hitCircles_.erase(itr);
		} else {
			hitCircles_[i]->draw(graphics, msCounter_);
		}
		--itr;
	}
	graphics.flip();
}

void Game::update(float elapsedTime) {
}