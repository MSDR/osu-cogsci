
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
void Game::fillCircleVec(const std::string& fileName, Sprite* circleSprite, Sprite* circleOverlay, Sprite* approachCircle, Graphics& graphics) {
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
			hitCircles_.push_back(tmpCircle);
			//std::cout << "Loaded hitcircle" << std::endl;

			//Create line between hitcircles
			if (hitCircles_.size() > 1 && currCombo != 1) {
				lineSpriteCoords_.push_back(Vector2(hitCircles_[hitCircles_.size() - 2]->getCoords().x, hitCircles_[hitCircles_.size() - 2]->getCoords().y));
				lineOffsets_.push_back(std::pair<int, int>(hitCircles_[hitCircles_.size() - 2]->getOffset(), hitCircles_[hitCircles_.size() - 1]->getOffset()));
				int distX = hitCircles_[hitCircles_.size() - 2]->getCoords().x - hitCircles_[hitCircles_.size() - 1]->getCoords().x;
				int distY = hitCircles_[hitCircles_.size() - 2]->getCoords().y - hitCircles_[hitCircles_.size() - 1]->getCoords().y;
				int length = sqrt(pow(distX, 2) + pow(distY, 2));
				float angle;
				if (distX != 0) {
					if (distY == 0) {
						if (distX > 0) {
							angle = 0;
						}
						else {
							angle = M_PI;
						}
					}
					else {
						angle = tan(distY / distX);
					}
				}
				else {
					if (distY > 0) {
						angle = M_PI / 2;
					}
					else {
						angle = 3 * M_PI / 2;
					}
				}
				lineSprites_.push_back(new Sprite(graphics, "Skin/followpoint-3@2x.png", 0, 0, length, 15));
				lineAngles_.push_back(angle);
				std::cout << "LINE ADDED" << std::endl;
			}
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
	std::string songName = "Beatmaps/Undertale";
	Vector2f diff = assignDifficulty(songName, ms300_, ms100_, ms50_);
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

	//Initialize number and score assets
	for (int i = 0; i < 10; i++) {
		numSprite_[i] = new Sprite(graphics, "Skin/default-" + std::to_string(i) + "@2x.png", 0, 0, 40, 52);
		scoreSprite_[i] = new Sprite(graphics, "Skin/score-" + std::to_string(i) + ".png", 0, 0, 36, 60);
	}
	//Initialize other score assets
	scoreSprite_[10] = new Sprite(graphics, "Skin/score-dot.png", 0, 0, 35, 60);
	scoreSprite_[11] = new Sprite(graphics, "Skin/score-percent.png", 0, 0, 35, 60);

	//Initialize circle score assets
	sprite300_ = new Sprite(graphics, "Skin/hit300.png", 0, 0, 30, 30);
	sprite100_ = new Sprite(graphics, "Skin/hit100.png", 0, 0, 30, 30);
	sprite50_ = new Sprite(graphics, "Skin/hit50.png", 0, 0, 30, 30);
	sprite0_ = new Sprite(graphics, "Skin/hit0.png", 0, 0, 30, 30);

	//Fill the hitCircles_ vector with the set beatmap
	fillCircleVec(songName, hitCircle_, hitCircleOverlay_, approachCircle_, graphics);
	int LAST_UPDATE_TIME = SDL_GetTicks();

	//Load custom cursor
	SDL_Surface* cursorSurface = IMG_Load("Skin/cursor.png");
	if (cursorSurface) {
		SDL_Cursor* cursor = SDL_CreateColorCursor(cursorSurface, 0, 0);
		if (cursor) SDL_SetCursor(cursor);
	}

	//Initialize variables needed to calculate accuracy and score
	num300_ = 0;
	num100_ = 0;
	num50_ = 0;
	numMiss_ = 0;
	notesPassed_ = 0;
	accuracy_ = 100;

	//Start the audio
	Mix_OpenAudio(22050, AUDIO_S16SYS, 2, 640);
	music = Mix_LoadMUS((songName + ".mp3").c_str());
	if(!music) {
		std::cout << "Mix_LoadMUS(\"music.mp3\"): " << Mix_GetError() << std::endl;
	}

	Mix_PlayMusic(music, 0);

	//Mix_LoadMUS("music.mp3"): Failed loading libmpg123-0.dll: The specified module could not be found.
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
					if (std::abs((int)msCounter_ - hitCircles_[i]->getOffset()) < ms300_) {
						num300_++;
						hitSprites_.push_back(std::pair<Sprite*, int>(new Sprite(graphics, "Skin/hit300.png", 0, 0, 50, 50), msCounter_));
						hitSpriteCoords_.push_back(Vector2(hitCircles_[i]->getCoords().x - 60, hitCircles_[i]->getCoords().y - 30));
						Mix_PlayChannel(-1, normalHitNormal_, 0);
					}
					else if (std::abs((int)msCounter_ - hitCircles_[i]->getOffset()) < ms100_) {
						num100_++;
						hitSprites_.push_back(std::pair<Sprite*, int>(new Sprite(graphics, "Skin/hit100.png", 0, 0, 50, 50), msCounter_));
						hitSpriteCoords_.push_back(Vector2(hitCircles_[i]->getCoords().x - 60, hitCircles_[i]->getCoords().y - 30));
						Mix_PlayChannel(-1, normalHitNormal_, 0);
					}
					else if (std::abs((int)msCounter_ - hitCircles_[i]->getOffset()) < ms50_) {
						num50_++;
						hitSprites_.push_back(std::pair<Sprite*, int>(new Sprite(graphics, "Skin/hit50.png", 0, 0, 50, 50), msCounter_));
						hitSpriteCoords_.push_back(Vector2(hitCircles_[i]->getCoords().x - 60, hitCircles_[i]->getCoords().y - 30));
						Mix_PlayChannel(-1, normalHitNormal_, 0);
					}
					else {
						numMiss_++;
					}
					notesPassed_++;
					accuracy_ = (num300_ + (num100_ * .3) + (num50_ * .15)) / notesPassed_;
					accuracy_ *= 100;
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
//Not currently working
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
		if (hitCircles_[i]->getOffset() - msCounter_ < -ms50_ || hitCircles_[i]->clicked_) {
			if(!hitCircles_[i]->clicked_) {
				//begin drawing an X for miss
				hitSprites_.push_back(std::pair<Sprite*, int>(new Sprite(graphics, "Skin/hit0.png", 0, 0, 50, 50), msCounter_));
				hitSpriteCoords_.push_back(Vector2(hitCircles_[i]->getCoords().x - 60, hitCircles_[i]->getCoords().y - 30));
			}

			//Update accuracy if expired
			if (!hitCircles_[i]->clicked_) {
				numMiss_++;
				notesPassed_++;
				accuracy_ = (num300_ + (num100_ * .3) + (num50_ * .15)) / notesPassed_;
				accuracy_ *= 100;
			}
			delete hitCircles_[i];
			itr = hitCircles_.erase(itr);

			
		} else {
			hitCircles_[i]->draw(graphics, msCounter_, numSprite_);
		}
		--itr;
	}

	std::vector<std::pair<Sprite*, int>>::iterator spriteItr = hitSprites_.end();
	std::vector<Vector2>::iterator spriteCoordsItr = hitSpriteCoords_.end();
	--spriteItr;
	--spriteCoordsItr;
	//Loop through hitSprites_ vector, delete the sprite after certain amount of time. Otherwise, draw it.
	for (int i = hitSprites_.size() - 1; i >= 0; --i) {
		if (msCounter_ - hitSprites_[i].second > 1000) {
			delete hitSprites_[i].first;
			spriteItr = hitSprites_.erase(spriteItr);
			spriteCoordsItr = hitSpriteCoords_.erase(spriteCoordsItr);
		}
		else {
			hitSprites_[i].first->draw(graphics, hitSpriteCoords_[i].x, hitSpriteCoords_[i].y, true, 1.0, 1.0, 180);
		}
		--spriteItr;
		--spriteCoordsItr;
	}

	std::vector<Sprite*>::iterator lineSpriteItr = lineSprites_.end();
	std::vector<Vector2>::iterator lineCoordsItr = lineSpriteCoords_.end();
	std::vector<std::pair<int, int> >::iterator offsetItr = lineOffsets_.end();
	std::vector<float>::iterator angleItr = lineAngles_.end();
	--lineSpriteItr;
	--lineCoordsItr;
	--offsetItr;
	--angleItr;
	//Loop through hitSprites_ vector, delete the sprite after certain amount of time. Otherwise, draw it.
	for (int i = lineSprites_.size() - 1; i >= 0; --i) {
		if (lineOffsets_[i].second - msCounter_ < -ms50_) {
			delete lineSprites_[i];
			lineSpriteItr = lineSprites_.erase(lineSpriteItr);
			lineCoordsItr = lineSpriteCoords_.erase(lineCoordsItr);
			offsetItr = lineOffsets_.erase(offsetItr);
			angleItr = lineAngles_.erase(angleItr);
		}
		else if (msCounter_ > lineOffsets_[i].first) {
			//Once the overloaded draw function that allows rotation is made, put lineAngles_[i] as the last parameter
			lineSprites_[i]->draw(graphics, lineSpriteCoords_[i].x, lineSpriteCoords_[i].y, true, 1.0, 1.0, 180);
		}
		--lineSpriteItr;
		--lineCoordsItr;
		--offsetItr;
		--angleItr;
	}

	//Create accuracy string
	std::string accStr;
	if (accuracy_ == 100) {
		accStr = "100.00%";
	}
	else if (accuracy_ >= 10) {
		accStr = std::to_string(accuracy_).substr(0, 5) + "%";
	}
	else {
		accStr = "0" + std::to_string(accuracy_).substr(0, 4) + "%";
	}

	//Draw accuracy at the top right
	char currChar;
	int xVal = SCREEN_WIDTH - 40;
	for (int i = accStr.size() - 1; i >= 0; i--) {
		currChar = accStr[i];
		if (isdigit(currChar)) {
			int num = currChar - 48;
			scoreSprite_[num]->draw(graphics, xVal, 20, true, 1.0, 1.0, 180);
		}
		else if (currChar == '.') {
			scoreSprite_[10]->draw(graphics, xVal, 20, true, 1.0, 1.0, 180);
		}
		else {
			scoreSprite_[11]->draw(graphics, xVal, 20, true, 1.0, 1.0, 180);
		}
		xVal -= 40;
	}

	graphics.flip();
}

void Game::update(float elapsedTime) {
}
