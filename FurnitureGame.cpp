/*
 
Copyright (C) <2013> <Ryan Krumins ryan.krumins@gmail.com>

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is furnished
to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

*/

#include "FurnitureGame.h"
#include "Level.h"

#include <SDL.h>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <fstream>

extern SDL_Rect fontRects[0x5E]; // 0x7E - 0x20
static const unsigned int BackgroundColor = 0x009930;
static SDL_Rect furnitureRects[] = {
	{  0,  0,  0,  0}, // Empty
	{  0,  0, 84, 42}, // Blue Table
	{  0, 42, 84, 42}, // Office Desk
	{ 84,  0, 42, 84}, // Piano
	{126 , 0, 42,126}, // Sofa
	{  0, 84,126, 42}, // BookCase
};

extern Level allLevels[LEVEL_COUNT];

FurnitureGame::FurnitureGame() :
	screen_(0), houseSurface_(0), furnitureSurface_(0), fontSurface_(0),
	level_(0), moves_(0), startTime_(0),
	currentLevel_(allLevels[0]), currentGame_(allLevels[0]),
	gameTimer_(0), currentState_(Menu), selectedMenuItem_(-1),
	selectedPlayItem_(-1), selectedLevelItem_(-1), selectedHighScoreItem_(-1),
	highScorePage_(0)
{
#ifdef WIN32
	const char *home = getenv("TEMP");
#else
	const char *home = getenv("HOME");
#endif
	std::string filename = home ? home : "";
	filename += "/.furniturefrenzy";
	std::ifstream input(filename.c_str());
	if (!input.fail()) {
		for (int index = 0; index < LEVEL_COUNT; ++index) {
			input >> highScore_[index].moves_;
			input >> highScore_[index].elapsedSeconds_;
		}
		input.close();
	}
}

void FurnitureGame::run()
{
	if (!initializeWindow())
		return;

	if (!loadImages())
		return;

	loadLevel(0);
	enterState(Menu);

	// do event loop
	SDL_Event event;
	while (SDL_WaitEvent(&event)) {
		// process event
		if (event.type == SDL_QUIT)
			break;

		if (currentState_ == Menu) {
			// Menu State
			if (event.type == SDL_MOUSEMOTION) {
				int newSelectedItem = -1;
				if (event.motion.x >= 125 && event.motion.x <= (500-125) &&
						event.motion.y >= (55+29*2+2) &&
						event.motion.y <= (55+29*6+2)) {
					newSelectedItem = (event.motion.y - (55+29*2+2)) / 29;
				}
				if (newSelectedItem != selectedMenuItem_) {
					selectedMenuItem_ = newSelectedItem;
					blitMenuSelection();
					SDL_Flip(screen_);
				}
			} else if (event.type == SDL_MOUSEBUTTONDOWN) {
				switch (selectedMenuItem_) {
					case 0: // Play
						loadLevel(0);
						enterState(Play);
						continue;
					case 1: // Level Select
						enterState(LevelSelect);
						break;
					case 2: // High Score
						enterState(HighScoreState);
						break;
					case 3: // Quit
						return;
				}
			}
		} else if (currentState_ == Play) {
			// Play State
			if (event.type == SDL_USEREVENT) {
				gameTimerCallback();
			} else if (event.type == SDL_MOUSEBUTTONDOWN) {
				// handle menu button clicks
				if (selectedPlayItem_ != -1) {
					switch (selectedPlayItem_) {
						case 0:
							resetLevel();
							blitMovesNumber();
							blitMovesNumber();
							blitGameTime();
							blitFurniture();
							SDL_Flip(screen_);
							break;	
						case 1:
							enterState(Menu);
							break;
						default:
							break;
					}
					continue;
				}

				// handle clicking on the furniture
				int mx, my;
				mx = (event.button.x - 193) / 42;
				my = (event.button.y - 13) / 42;
				if (mx >= 0 && mx < 6 && my >= 0 && my < 6) {
					gameX_ = mx;
					gameY_ = my;
				} else {
					gameX_ = -1;
					gameY_ = -1;
				}
			} else if (event.type == SDL_MOUSEBUTTONUP) {
				gameX_ = -1;
				gameY_ = -1;
			} else if (event.type == SDL_MOUSEMOTION) {
				// handle possible menu item selection change
				int newSelectedItem = -1;
				if (event.motion.x <= 160 && event.motion.y >= 260) {
					newSelectedItem = (event.motion.y - 260) / 29;
				}
				if (newSelectedItem != selectedPlayItem_) {
					selectedPlayItem_ = newSelectedItem;
					blitPlayButtonSelection();
					SDL_Flip(screen_);
					continue;
				}

				// handle moving game piece if possbile
				if (gameX_ >= 0 && gameY_ >= 0) {
					int mx, my;
					mx = (event.motion.x - 193) / 42;
					my = (event.motion.y - 13) / 42;
					if (mx != gameX_ || my != gameY_) {
						Level::Direction dir;
						if (mx != gameX_)
							dir = (mx < gameX_) ? Level::Left : Level::Right;
						else
							dir = (my < gameY_) ? Level::Up : Level::Down;
						if (currentGame_.canMove(gameX_, gameY_, dir)) {
							currentGame_.move(gameX_, gameY_, dir);
							gameX_ = mx;
							gameY_ = my;
							++moves_;
							if (startTime_ == 0)
								startTime_ = time(0);
							if (currentGame_.complete_) {
								enterState(LevelComplete);
								continue;
							}
							blitMovesNumber();
							blitFurniture();
							SDL_Flip(screen_);
						}
					}	
				}
			}
		} else if (currentState_ == LevelComplete) {
			if (event.type == SDL_MOUSEBUTTONDOWN) {
				if (level_+1 == LEVEL_COUNT) {
					enterState(Menu);
				} else {
					loadLevel(level_+1);
					enterState(Play);
				}
			}
		} else if (currentState_ == LevelSelect) {
			if (event.type == SDL_MOUSEMOTION) {
				// handle possible menu item selection change
				int newSelectedItem = -1;
				if (event.motion.x >= 103 && event.motion.x <= 103+5*63 &&
						event.motion.y >= 68 && event.motion.y <= 68+5*30) {
					newSelectedItem = (event.motion.x - 103) / 63 +
							(event.motion.y - 68) / 30 * 5;
				} else if (event.motion.x >= 65 &&
						event.motion.x <= 65+160 &&
						event.motion.y >= 246 && event.motion.y <= 246+30) {
					newSelectedItem = LEVEL_COUNT;
				}
				if (newSelectedItem != selectedLevelItem_) {
					selectedLevelItem_ = newSelectedItem;
					blitLevelSelectSelection();
					SDL_Flip(screen_);
					continue;
				}
			} else if (event.type == SDL_MOUSEBUTTONDOWN) {
				if (selectedLevelItem_ >= 0 &&
						selectedLevelItem_ < LEVEL_COUNT) {
					loadLevel(selectedLevelItem_);
					enterState(Play);
					continue;
				} else if (selectedLevelItem_ == LEVEL_COUNT) {
					enterState(Menu);
					continue;
				}
			}
		} else if (currentState_ == HighScoreState) {

			if (event.type == SDL_MOUSEMOTION) {
				// handle possible menu item selection change
				int newSelectedItem = -1;
				if (event.motion.x >= 72 && event.motion.x <= 112 &&
						event.motion.y >= 240 && event.motion.y <= 270 &&
						highScorePage_ != 0) {
					newSelectedItem = 0;
				} else if (event.motion.x >= 172 && event.motion.x <= 332 &&
						event.motion.y >= 240 && event.motion.y <= 270) {
					newSelectedItem = 1;
				} else if (event.motion.x >= 382 && event.motion.x <= 422 &&
						event.motion.y >= 240 && event.motion.y <= 270 &&
						highScorePage_ != 4) {
					newSelectedItem = 2;
				}
				if (newSelectedItem != selectedHighScoreItem_) {
					selectedHighScoreItem_ = newSelectedItem;
					blitHighScoreSelection();
					SDL_Flip(screen_);
					continue;
				}
			} else if (event.type == SDL_MOUSEBUTTONDOWN) {
				switch (selectedHighScoreItem_) {
					case 0:
						--highScorePage_;
						if (highScorePage_ < 0)
							highScorePage_ = 0;
						blitMenuBox();
						blitHighScore();
						blitHighScoreSelection();
						SDL_Flip(screen_);
						break;
					case 1:
						enterState(Menu);
						continue;
					case 2:
						++highScorePage_;
						if (highScorePage_ >= 5)
							highScorePage_ = 4;
						blitMenuBox();
						blitHighScore();
						blitHighScoreSelection();
						SDL_Flip(screen_);
						break;
					default:
						break;
				}
			}
		}

		}
	}

bool FurnitureGame::initializeWindow()
{
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_AUDIO |
			SDL_INIT_NOPARACHUTE) < 0) {
		return false;
	}

	// set the events we want to see
	SDL_EventState((int)SDL_ALLEVENTS, SDL_DISABLE);
	SDL_EventState((int)SDL_KEYDOWN, SDL_ENABLE);
	SDL_EventState((int)SDL_KEYUP, SDL_ENABLE);
	SDL_EventState((int)SDL_MOUSEMOTION, SDL_ENABLE);
	SDL_EventState((int)SDL_MOUSEBUTTONDOWN, SDL_ENABLE);
	SDL_EventState((int)SDL_MOUSEBUTTONUP, SDL_ENABLE);
	SDL_EventState((int)SDL_QUIT, SDL_ENABLE);

	screen_ = SDL_SetVideoMode( 500, 320, 24, SDL_SWSURFACE | SDL_RESIZABLE);
	if (screen_ == 0) {
		std::cerr << SDL_GetError() << std::endl;
		return false;
	}

	SDL_WM_SetCaption("Furniture Frenzy", "Furniture Frenzy");
	SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY, SDL_DEFAULT_REPEAT_INTERVAL);

	return true;
}

bool FurnitureGame::loadImages()
{
	extern SDL_Surface *houseImageSurface();
	extern SDL_Surface *furnitureImageSurface();
	extern SDL_Surface *fontImageSurface();

	houseSurface_ = houseImageSurface();
	if (!houseSurface_)
		return false;
	furnitureSurface_ = furnitureImageSurface();
	if (!houseSurface_)
		return false;
	fontSurface_ = fontImageSurface();
	if (!houseSurface_)
		return false;
	return true;
}

void FurnitureGame::enterState(GameState state)
{
	leaveState(currentState_);
	currentState_ = state;
	switch (state) {
		case Menu:
			selectedMenuItem_ = -1;
			blitBackground();
			blitMenuBox();
			blitMenuText();
			blitMenuSelection();
			SDL_Flip(screen_);
			break;
		case Play:
			selectedPlayItem_ = -1;
			blitBackground();
			blitBackgroundText();
			blitPlayButtons();
			blitPlayButtonSelection();
			blitFurniture();
			SDL_Flip(screen_);
			gameX_ = -1;
			gameY_ = -1;
			gameTimer_ = SDL_AddTimer(1000, (SDL_NewTimerCallback)gameTimerCallback_s, this);
			break;
		case LevelComplete:
			endTime_ = time(0);
			if (endTime_ <= startTime_)
				endTime_ = startTime_+1;
			blitBackground();
			blitMenuBox();
			blitLevelComplete();
			SDL_Flip(screen_);

			if (isNewHighScore()) {
				applyNewHighScore();
			}
			break;
		case LevelSelect:
			selectedLevelItem_ = -1;
			blitBackground();
			blitMenuBox();
			blitLevelSelect();
			blitLevelSelectSelection();
			SDL_Flip(screen_);
			break;
		case HighScoreState:
			selectedHighScoreItem_ = -1;
			highScorePage_ = 0;
			blitBackground();
			blitMenuBox();
			blitHighScore();
			blitHighScoreSelection();
			SDL_Flip(screen_);
			break;
		default:
			break;
	}
}

void FurnitureGame::leaveState(GameState state)
{
	switch (state) {
		case Play:
			SDL_RemoveTimer(gameTimer_);
			break;
		default:
			break;
	}
}

void FurnitureGame::loadLevel(int levelNumber)
{
	level_ = levelNumber;
	currentLevel_ = allLevels[levelNumber];
	resetLevel();
}

void FurnitureGame::resetLevel()
{
	moves_ = 0;
	startTime_ = 0;
	currentGame_ = currentLevel_;
}

void FurnitureGame::blitMenuBox()
{
	static SDL_Rect menuRegion = {52, 52, 398, 218};
	SDL_FillRect(screen_, &menuRegion, 0xFFFF99);

	static SDL_Rect l1 = {48, 48, 404, 4};
	static SDL_Rect l2 = {48, 270, 404, 4};
	static SDL_Rect l3 = {48, 48, 4, 224};
	static SDL_Rect l4 = {448, 48, 4, 224};
	SDL_FillRect(screen_, &l1, 0);
	SDL_FillRect(screen_, &l2, 0);
	SDL_FillRect(screen_, &l3, 0);
	SDL_FillRect(screen_, &l4, 0);
}

void FurnitureGame::blitMenuText()
{
	blitString(145, 55, "Furniture Frenzy");

	blitString(145, 55+29*2, "Play");
	blitString(145, 55+29*3, "Level Select");
	blitString(145, 55+29*4, "High Scores");
	blitString(145, 55+29*5, "Exit");
}

void FurnitureGame::blitLevelComplete()
{
	if (level_+1 == LEVEL_COUNT) {
		blitString(80, 55, "*** Final Level Complete ***");
		blitString(90, 55+29*6, "Click to return to menu");
	} else {
		blitString(145, 55, "Level Complete!");
		blitString(145, 55+29*6, "Click to continue");
	}

	std::stringstream ostr;
	ostr << moves_ << " moves taken";
	blitString(145, 55+29*2, ostr.str());

	ostr.str(std::string());
	time_t seconds = endTime_ - startTime_;

	if (seconds >= 60*60) {
		ostr << (seconds / (60*60)) << "h ";
	}
	if (seconds >= 60) {
		ostr << ((seconds % (60*60)) / 60) << "m ";
	}
	ostr << (seconds % 60) << "s elapsed";
	blitString(145, 55+29*3, ostr.str());
	
	if (isNewHighScore()) {
		blitString(100, 55+29*4+14, "*** New high score ***");
	}
}

bool FurnitureGame::isNewHighScore() const
{
	time_t elapsed = endTime_ - startTime_;
	
	if (highScore_[level_].moves_ == 0)
		return true;

	if (moves_ < highScore_[level_].moves_ ||
			elapsed < highScore_[level_].elapsedSeconds_)
		return true;
	return false;
}

void FurnitureGame::applyNewHighScore()
{
	if (moves_ < highScore_[level_].moves_ || highScore_[level_].moves_ == 0)
		highScore_[level_].moves_ = moves_;

	time_t elapsed = endTime_ - startTime_;

	if (elapsed < highScore_[level_].elapsedSeconds_ || highScore_[level_].elapsedSeconds_ == 0)
		highScore_[level_].elapsedSeconds_ = elapsed;

	std::stringstream ostr;
	for (int index = 0; index < LEVEL_COUNT; ++index) {
		ostr << highScore_[index].moves_ << " " <<
				highScore_[index].elapsedSeconds_ << std::endl;
	}

#ifdef WIN32
	const char *home = getenv("TEMP");
#else
	const char *home = getenv("HOME");
#endif
	std::string filename = home ? home : "";
	filename += "/.furniturefrenzy";
	std::ofstream output(filename.c_str());
	if (!output.fail()) {
		output << ostr.str();
		output.close();
	}
}

void FurnitureGame::blitLevelSelect()
{
	for (int index = 0; index < LEVEL_COUNT; ++index) {
		int x = 123 + (index % 5) * 63;
		int y = 62 + (index / 5) * 30;

		std::stringstream ostr;
		ostr << (index+1);
		blitString(x, y, ostr.str());
	}
	blitString(85, 240, "Main Menu");
}

void FurnitureGame::blitLevelSelectSelection()
{
	for (int index = 0; index < LEVEL_COUNT; ++index) {
		if (index != 0 && highScore_[index-1].moves_ == 0) {
			continue;
		}

		int x = 103 + (index % 5) * 63;
		int y = 68 + (index / 5) * 30;

		SDL_Rect mi = {(Sint16)x, (Sint16)y, 15, 15};

		unsigned int color = 0x999999;
		if (index == selectedLevelItem_)
			color = 0xFF3333;
		SDL_FillRect(screen_, &mi, color);
	}

	SDL_Rect mi = {65, 246, 15, 15};
	unsigned int color = 0x999999;
	if (selectedLevelItem_ == 25)
		color = 0xFF3333;
	SDL_FillRect(screen_, &mi, color);
}

void FurnitureGame::blitHighScore()
{
	int pos = highScorePage_*5;
	for (int index = 0; index < 5; ++ index) {
		std::stringstream ostr;
		ostr << (pos+1) << ".  ";
		if (highScore_[pos].moves_ > 0) {
			ostr << std::setw(3) << highScore_[pos].moves_ <<
					std::setw(0) << " moves  ";

			time_t seconds = highScore_[pos].elapsedSeconds_;
			if (seconds >= 60*60) {
				ostr << (seconds / (60*60)) << "h ";
			}
			if (seconds >= 60) {
				ostr << ((seconds % (60*60)) / 60) << "m ";
			}
			ostr << (seconds % 60) << "s";
		} else {
			ostr << "    -----";
		}

		blitString(62, index*30+62, ostr.str());
		++pos;
	}

	blitString(92, 240, "<");
	blitString(192, 240, "Main Menu");
	blitString(402, 240, ">");
}

void FurnitureGame::blitHighScoreSelection()
{
	static SDL_Rect mi[] = {
		{72, 246, 15, 15},
		{172, 246, 15, 15},
		{382, 246, 15, 15},
	};
	
	for (int index = 0; index < 3; ++index) {
		unsigned int color = 0x999999;
		if (index == selectedHighScoreItem_)
			color = 0xFF3333;
		if ((index == 0 && highScorePage_ == 0) ||
				(index == 2 && highScorePage_ == 4)) {
			color = 0xFFFF99;
		}
		SDL_FillRect(screen_, &mi[index], color);
	}
}

void FurnitureGame::blitMenuSelection()
{
	static SDL_Rect mi[] = {
		{125, 55+29*2+6, 15, 15},
		{125, 55+29*3+6, 15, 15},
		{125, 55+29*4+6, 15, 15},
		{125, 55+29*5+6, 15, 15},
	};
	
	for (int index = 0; index < 4; ++index) {
		unsigned int color = 0x999999;
		if (index == selectedMenuItem_)
			color = 0xFF3333;
		SDL_FillRect(screen_, &mi[index], color);
	}

}

void FurnitureGame::blitBackground()
{
	SDL_BlitSurface(houseSurface_, 0, screen_, 0);
}

void FurnitureGame::blitBackgroundText()
{
	blitString(5, 5, "Level");
	blitLevelNumber();

	blitString(5, 5+29*3, "Moves");
	blitMovesNumber();

	blitString(5, 5+29*6, "Time");
	blitGameTime();
}

void FurnitureGame::blitPlayButtons()
{
	blitString(25, 260, "Reset");
	blitString(25, 290, "Main Menu");
}

void FurnitureGame::blitPlayButtonSelection()
{
	static SDL_Rect mi[] = {
		{5, 266, 15, 15},
		{5, 296, 15, 15},
	};
	
	for (int index = 0; index < 2; ++index) {
		unsigned int color = 0x999999;
		if (index == selectedPlayItem_)
			color = 0xFF3333;
		SDL_FillRect(screen_, &mi[index], color);
	}
}

void FurnitureGame::blitLevelNumber()
{
	std::stringstream ostr;
	ostr << level_+1;

	static SDL_Rect textArea = {5, 5+29, 140, 29};
	SDL_FillRect(screen_, &textArea, BackgroundColor);
	blitString(textArea.x, textArea.y, ostr.str());
}

void FurnitureGame::blitMovesNumber()
{
	std::stringstream ostr;
	ostr << moves_;

	static SDL_Rect textArea = {5, 5+29*4, 140, 29};
	SDL_FillRect(screen_, &textArea, BackgroundColor);
	blitString(textArea.x, textArea.y, ostr.str());
}

void FurnitureGame::blitGameTime()
{
	std::stringstream ostr;
	time_t now = time(0);
	time_t seconds = now - startTime_;

	if (startTime_ == 0)
		seconds = 0;

	if (seconds >= 60*60) {
		ostr << (seconds / (60*60)) << "h ";
	}
	if (seconds >= 60) {
		ostr << ((seconds % (60*60)) / 60) << "m ";
	}
	ostr << (seconds % 60) << "s";

	static SDL_Rect textArea = {5, 5+29*7, 140, 29};
	SDL_FillRect(screen_, &textArea, BackgroundColor);
	blitString(textArea.x, textArea.y, ostr.str());
}

void FurnitureGame::blitFurniture()
{
	static SDL_Rect houseRect = {190, 10, 258, 258 };
	SDL_BlitSurface(houseSurface_, &houseRect, screen_, &houseRect);

	unsigned int upper = currentGame_.x_ * currentGame_.y_;
	for (unsigned int index = 0; index < upper; ++index) {
		Level::Obstacle obs = currentGame_.levelData_[index];
		if (obs == Level::Empty)
			continue;

		SDL_Rect output = furnitureRects[obs];
		output.x = 193 + 42*(index % currentGame_.x_);
		output.y = 13 + 42*(index / currentGame_.x_);

		SDL_BlitSurface(furnitureSurface_, &furnitureRects[obs], screen_,
				&output);
	}
}

void FurnitureGame::blitString(unsigned int x, unsigned int y,
		const std::string &text)
{
	SDL_Rect output;

	for (std::string::const_iterator iter = text.begin();
			iter != text.end(); ++iter) {
		unsigned int glyph = (*iter)-0x20;
		
		if (glyph >= 0x7E) {
			std::cerr << "Skipping character: " << (glyph+0x20) << std::endl;
			continue;
		}

		output = fontRects[glyph];
		output.x = x;
		output.y = y;

		if (glyph != 0x20) {
			SDL_BlitSurface(fontSurface_, &fontRects[glyph], screen_,
					&output);
		}

		x += output.w;
	}
}

unsigned int FurnitureGame::gameTimerCallback_s(unsigned int interval,
		FurnitureGame *param)
{
	SDL_Event event;
	SDL_UserEvent userevent;
	userevent.type = SDL_USEREVENT;
	userevent.code = 0;
	userevent.data1 = 0;
	userevent.data2 = 0;
	event.type = SDL_USEREVENT;
	event.user = userevent;
	SDL_PushEvent(&event);

	return 1000;
}

void FurnitureGame::gameTimerCallback()
{
	blitGameTime();
	SDL_Flip(screen_);
}

int main(int argc, char **argv)
{
	FurnitureGame game;
	game.run();

	return 0;
}

