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
	msCounter_ = 0;
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

	/*sprite_ = Sprite(Graphics &graphics, const std::string &filePath, int sourceX, int sourceY, 
	int width, int height, float posX, float posY); */

	int LAST_UPDATE_TIME = SDL_GetTicks();

	//Load custom cursor
	SDL_Surface* cursorSurface = IMG_Load("Skin/cursor.png");
	if (cursorSurface) {
		std::cout << "success";
		SDL_Cursor* cursor = SDL_CreateColorCursor(cursorSurface, 0, 0);
		if (cursor) {
			SDL_SetCursor(cursor);
			std::cout << "set cursor\n";
		}
	}
	else {
		std::cout << IMG_GetError();
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

		if (input.wasKeyPressed(SDL_SCANCODE_DELETE)) {
			return;
		}

		if (input.wasKeyPressed(SDL_SCANCODE_X) || input.wasKeyPressed(SDL_SCANCODE_Z)) {
			Mix_Chunk* hitSound = Mix_LoadWAV("Skin/normal-hitnormal.wav");
			Mix_PlayChannel(-1, hitSound, 0);
		}
		const int CURRENT_TIME_MILLIS = SDL_GetTicks();
		int ELAPSED_TIME_MILLIS = CURRENT_TIME_MILLIS - LAST_UPDATE_TIME;
		msCounter_ += ELAPSED_TIME_MILLIS;
		update(std::min(ELAPSED_TIME_MILLIS, MAX_FRAME_TIME));
		LAST_UPDATE_TIME = CURRENT_TIME_MILLIS;
		draw(graphics);
		//std::cout << msCounter_ << std::endl;
	}//end game loop
}

void Game::draw(Graphics &graphics) {
	graphics.clear();
	sprite_.draw(graphics, sprite_.getX(), sprite_.getY());
	//Text msCounter;
	//std::string str = "Impact.ttf";
	//msCounter.loadFont(str, 24);
	//msCounter.update(graphics, std::to_string(msCounter_), { 100,100,100,100 });
	//msCounter.draw(graphics, 100, 100, 0.0);
	graphics.flip();
}

void Game::update(float elapsedTime) {
	sprite_.update(elapsedTime);
}