
#include "game.h"

namespace {
	const int FPS = 60;
	const int MAX_FRAME_TIME = 75;
}

namespace input_keys {
	SDL_Scancode key_name = SDL_SCANCODE_0;
}

//Finds and returns a Vector2f(CircleSize, ApproachRate)
Vector2f assignDifficulty(const std::string& fileName) {
	std::ifstream inFile(fileName + ".txt");
	if (!inFile) {
		std::cout << "File not found!" << std::endl;
	}
	std::string tmpStr = "";
	std::vector<std::string> line;
	int tmpNum;

	//Advance to setup metrics
	while (tmpStr != "[Difficulty]") {
		inFile >> tmpStr;
	}
	inFile >> tmpStr;
	inFile >> tmpStr;

	//Calculate circle radius: https://osu.ppy.sh/community/forums/topics/311844
	float CS = std::atof(tmpStr.substr(11).c_str());
	//std::cout << tmpStr.substr(11) << std::endl;
	std::cout << "CS: " << CS << " HIT_RADIUS: " << HIT_CIRCLE_RADIUS << std::endl;
	inFile >> tmpStr;
	inFile >> tmpStr;

	//Calculate approach rate: https://osu.ppy.sh/help/wiki/Beatmapping/Approach_rate
	float AR = std::atof(tmpStr.substr(13).c_str());
	//std::cout << tmpStr.substr(13) << std::endl;

	std::cout << "AR: " << AR << " APPROACH_RATE: " << APPROACH_CIRCLE_RATE << std::endl;

	return Vector2f(CS, AR);
}

//Helper function used in parsing input
std::vector<std::string> splitStr(std::string& strIn) {
	std::stringstream sstream(strIn);
	std::string tmpSubstr;	
	std::vector<std::string> substrVec;
	//Break the input string up at each comma and put into vector
	while (std::getline(sstream, tmpSubstr, ',')) {
		substrVec.push_back(tmpSubstr);
	}
	return substrVec;
}

//Fill the input vector with hitcircles from the input beatmap
void fillCircleVec(const std::string& fileName, std::vector<HitCircle*>& hitCircles, Sprite* circleSprite, Sprite* circleOverlay, Sprite* approachCircle) {
	std::ifstream inFile(fileName + ".txt");
	if (!inFile) {
		std::cout << "File not found!" << std::endl;
	}
	std::string tmpStr = "";
	std::vector<std::string> line;
	int tmpNum;

	//Go through file until it finds the hitcircles
	while (tmpStr != "[HitObjects]") {
		inFile >> tmpStr;
	}

	//Start reading in hitcircles
	inFile >> tmpStr;
	while (!inFile.eof() && isdigit(tmpStr[0])) {
		//std::cout << "string is " << tmpStr << std::endl;
		line = splitStr(tmpStr);
		//std::cout << "Vec size is " << std::to_string(line.size()) << std::endl;
		if (line.size() > 1) {
			if (line[3] == "1") {
				tmpNum = 1;
			}
			else {
				tmpNum = 2;
			}
			hitCircles.push_back(new HitCircle(circleSprite, circleOverlay, approachCircle, stoi(line[2]), Vector2(stoi(line[0]), stoi(line[1])), tmpNum));
			//std::cout << "Loaded hitcircle" << std::endl;
		}
		inFile >> tmpStr;
	}
}


Game::Game() {
	//Initialize SDL, local variables
	SDL_Init(SDL_INIT_EVERYTHING);
	IMG_Init(IMG_INIT_PNG);
	Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);
	msCounter_ = 000;
	beatmap_ = -1;

	gameLoop();

	//Close SDL
	IMG_Quit();
	SDL_Quit();
}


//The main game loop. Once exited, the program closes.
void Game::gameLoop() {
	std::string songName = "Beatmaps/Algorhythm";
	Vector2f diff = assignDifficulty(songName);
	Graphics graphics(diff.x, diff.y);
	std::cout << SCREEN_WIDTH << " " << SCREEN_HEIGHT << " " << COORDINATE_SCALE << std::endl;
	Input input;
	SDL_Event event;
	Mix_Music* music = NULL;

	normalHitNormal_ = Mix_LoadWAV("Skin/normal-hitnormal.wav");

	//Initialize hitCircle assets
	hitCircle_ = new Sprite(graphics, "Skin/hitcircle@2x.png", 0, 0, HIT_CIRCLE_RADIUS*2, HIT_CIRCLE_RADIUS*2);
	hitCircleOverlay_ = new Sprite(graphics, "Skin/hitcircleoverlay@2x.png", 0, 0, HIT_CIRCLE_RADIUS*2, HIT_CIRCLE_RADIUS*2);
	approachCircle_ = new Sprite(graphics, "Skin/approachcircle@2x.png", 0, 0, HIT_CIRCLE_RADIUS*2, HIT_CIRCLE_RADIUS*2);

	//Fill the hitCircles_ vector with the set beatmap
	fillCircleVec(songName, hitCircles_, hitCircle_, hitCircleOverlay_, approachCircle_);
	int LAST_UPDATE_TIME = SDL_GetTicks();

	//Load custom cursor
	SDL_Surface* cursorSurface = IMG_Load("Skin/cursor.png");
	if (cursorSurface) {
		SDL_Cursor* cursor = SDL_CreateColorCursor(cursorSurface, 0, 0);
		if (cursor) SDL_SetCursor(cursor);
	}

	//Start the audio
	/*Mix_OpenAudio(22050, AUDIO_S16SYS, 2, 640);
	music = Mix_LoadMUS("Beatmaps/Pokemon Rap.mp3");
	Mix_PlayMusic(music, 0);*/


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

		//Handle clicking
		if (input.wasKeyPressed(SDL_SCANCODE_X) || input.wasKeyPressed(SDL_SCANCODE_Z)) {
			for (int i = 0; i < hitCircles_.size(); ++i) {
				if (hitCircles_[i]->getOffset() - msCounter_ >= APPROACH_CIRCLE_RATE) {
					break; //Circle's too far in the future to be clicked
				}

				//Figure out where the cursor's center is on screen
				int mouseX = 0;
				int mouseY = 0;
				SDL_GetMouseState(&mouseX, &mouseY);
				mouseX += cursorSurface->w / 2;
				mouseY += cursorSurface->h / 2;
				//std::cout << std::sqrt(std::pow(hitCircles_[i]->getCoords().x - mouseX, 2) + std::pow(hitCircles_[i]->getCoords().y - mouseY, 2)) << std::endl;

				//Determine if click is within the circle's radius
				if (std::sqrt(std::pow(hitCircles_[i]->getCoords().x - mouseX, 2) + std::pow(hitCircles_[i]->getCoords().y - mouseY, 2)) <= HIT_CIRCLE_RADIUS) {
					hitCircles_[i]->clicked_ = true;
					Mix_PlayChannel(-1, normalHitNormal_, 0);
					break;
				}
			}
		}

		//Update timing, draw circles
		int CURRENT_TIME_MILLIS = SDL_GetTicks();
		int ELAPSED_TIME_MILLIS = CURRENT_TIME_MILLIS - LAST_UPDATE_TIME;
		msCounter_ += ELAPSED_TIME_MILLIS;
		update(std::min(ELAPSED_TIME_MILLIS, MAX_FRAME_TIME));
		LAST_UPDATE_TIME = CURRENT_TIME_MILLIS;
		draw(graphics);
		//std::cout << msCounter_ << std::endl;
	}
	Mix_FreeMusic(music);
}

void Game::draw(Graphics &graphics) {
	graphics.clear();
	std::vector<HitCircle*>::iterator itr = hitCircles_.end();
	--itr;

	//Loop through hitCircles_ vector, delete if circle's been clicked or is expired. Otherwise, draw it.
	for(int i = hitCircles_.size()-1; i >=0 ; --i) {
		if (hitCircles_[i]->getOffset() - msCounter_ < -APPROACH_CIRCLE_RATE || hitCircles_[i]->clicked_) {
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
