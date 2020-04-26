
#include "game.h"

namespace {
	const int FPS = 60;
	const int MAX_FRAME_TIME = 75;
}

namespace input_keys {
	SDL_Scancode key_name = SDL_SCANCODE_0;
}

//Finds and returns a Vector2f(CircleSize, ApproachRate)
Vector2f assignDifficulty(const std::string& fileName, float& ms300, float& ms100, float& ms50) {
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

	//Calculate the timing for 300s, 100s, and 50s: https://osu.ppy.sh/help/wiki/Beatmap_Editor/Song_Setup#overall-difficulty
	int OD = std::stoi(tmpStr.substr(18).c_str());
	ms300 = 79.5 - (6 * OD);
	ms100 = 139.5 - (8 * OD);
	ms50 = 199.5 - (10 * OD);
	inFile >> tmpStr;

	//Calculate approach rate: https://osu.ppy.sh/help/wiki/Beatmapping/Approach_rate
	float AR = 7;
	if (tmpStr.substr(0, 12) == "ApproachRate") {
		AR = std::atof(tmpStr.substr(13).c_str());
		//std::cout << tmpStr.substr(13) << std::endl;

		std::cout << "AR: " << AR << " APPROACH_RATE: " << APPROACH_CIRCLE_RATE << std::endl;
	}

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

	//Go through file until it finds the hitcircles
	while (tmpStr != "[HitObjects]") {
		inFile >> tmpStr;
	}

	//Start reading in hitcircles
	int currCombo = 1;
	inFile >> tmpStr;
	while (!inFile.eof() && isdigit(tmpStr[0])) {
		//std::cout << "string is " << tmpStr << std::endl;
		line = splitStr(tmpStr);
		//std::cout << "Vec size is " << std::to_string(line.size()) << std::endl;
		if (line.size() > 1) {
			if (line[3] == "1") {
				currCombo++;
			}
			else {
				currCombo = 1;
			}
			HitCircle* tmpCircle = new HitCircle(circleSprite, circleOverlay, approachCircle, stoi(line[2]), Vector2(stoi(line[0]), stoi(line[1])), currCombo);
			hitCircles.push_back(tmpCircle);
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
	Vector2f diff = assignDifficulty(songName, ms300, ms100, ms50);
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

	//Initialize number assetts
	for (int i = 0; i < 9; i++) {
		numSprite[i] = new Sprite(graphics, "Skin/default-" + std::to_string(i) + "@2x.png", 0, 0, 40, 52);
	}

	//Fill the hitCircles_ vector with the set beatmap
	fillCircleVec(songName, hitCircles_, hitCircle_, hitCircleOverlay_, approachCircle_);
	int LAST_UPDATE_TIME = SDL_GetTicks();

	//Load custom cursor
	SDL_Surface* cursorSurface = IMG_Load("Skin/cursor.png");
	if (cursorSurface) {
		SDL_Cursor* cursor = SDL_CreateColorCursor(cursorSurface, 0, 0);
		if (cursor) SDL_SetCursor(cursor);
	}

	//Initialize variables needed to calculate accuracy and score
	num300 = 0;
	num100 = 0;
	num50 = 0;
	numMiss = 0;
	notesPassed = 0;
	accuracy = 100;

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
					//Add to num300, num100, num50, or numMiss and update accuracy
					if (std::abs((int)msCounter_ - hitCircles_[i]->getOffset()) < ms300) {
						num300++;
						Mix_PlayChannel(-1, normalHitNormal_, 0);
					}
					else if (std::abs((int)msCounter_ - hitCircles_[i]->getOffset()) < ms100) {
						num100++;
						Mix_PlayChannel(-1, normalHitNormal_, 0);
					}
					else if (std::abs((int)msCounter_ - hitCircles_[i]->getOffset()) < ms50) {
						num50++;
						Mix_PlayChannel(-1, normalHitNormal_, 0);
					}
					else {
						numMiss++;
					}
					notesPassed++;
					accuracy = (num300 + (num100 * .3) + (num50 * .15)) / notesPassed;
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

//Method taken from https://gist.github.com/stavrossk/5004111 to draw text with SDL
void drawText (SDL_Surface* screen, char* string, int size, int x, int y, int fR = 255, int fG = 255, int fB = 255, int bR = 0, int bG = 0, int bB = 0) {
	TTF_Font* font = TTF_OpenFont("ARIAL.TTF", size);

	SDL_Color foregroundColor = { fR, fG, fB };
	SDL_Color backgroundColor = { bR, bG, bB };

	SDL_Surface* textSurface
		= TTF_RenderText_Shaded
		(font, string, foregroundColor, backgroundColor);

	SDL_Rect textLocation = { x, y, 0, 0 };

	SDL_BlitSurface(textSurface, NULL, screen, &textLocation);

	SDL_FreeSurface(textSurface);

	TTF_CloseFont(font);
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
			hitCircles_[i]->draw(graphics, msCounter_, numSprite);
		}
		--itr;
	}

	/*
	//Draw the accuracy at the top right
	std::string accStr;
	if (accuracy == 100) {
		accStr = "100.00%"
	}
	else if (accuracy >= 10) {
		accStr = std::to_string(accuracy).substr(0, 4) + "%";
	}
	else {
		accStr = "0" + std::to_string(accuracy).substr(0, 4) + "%";
	}
	TTF_Font* font = TTF_OpenFont("ARIAL.TTF", 12);
	SDL_Color foregroundColor = { 255, 255, 255 };
	SDL_Color backgroundColor = { 0, 0, 0 };
	SDL_Surface* textSurface = TTF_RenderText_Shaded(font, "This is my text.", foregroundColor, backgroundColor);
	drawText(textSurface, accStr, SCREEN_WIDTH - 100, SCREEN_HEIGHT - 20); //This line doesn't work yet, doesn't look like it needs much more work, just make accStr into char*
	*/

	graphics.flip();
}

void Game::update(float elapsedTime) {
}
