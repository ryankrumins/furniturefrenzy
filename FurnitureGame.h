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

#ifndef _FURNITURE_GAME_H_
#define _FURNITURE_GAME_H_

#include "Level.h"

#include <SDL.h>
#include <string>

#define LEVEL_COUNT 25

class FurnitureGame
{
public:
	typedef enum {Menu, Play, LevelComplete, LevelSelect, HighScoreState} GameState;

	FurnitureGame();

	void run();	

protected:
	bool initializeWindow();
	bool loadImages();

	void leaveState(GameState state);
	void enterState(GameState state);
	void loadLevel(int levelNumber);
	void resetLevel();

	void blitMenuBox();
	void blitMenuText();
	void blitMenuSelection();

	void blitLevelComplete();
	bool isNewHighScore() const;
	void applyNewHighScore();

	void blitLevelSelect();
	void blitLevelSelectSelection();

	void blitHighScore();
	void blitHighScoreSelection();

	void blitBackground();
	void blitBackgroundText();
	void blitPlayButtons();
	void blitPlayButtonSelection();
	void blitString(unsigned int x, unsigned int y, const std::string &text);
	void blitLevelNumber();
	void blitMovesNumber();
	void blitGameTime();
	void blitFurniture();

	static unsigned int gameTimerCallback_s(unsigned int interval,
			FurnitureGame *param);
	void gameTimerCallback();


	SDL_Surface *screen_;
	SDL_Surface *houseSurface_, *furnitureSurface_, *fontSurface_;

	// state
	unsigned int level_;
	unsigned int moves_;
	time_t startTime_, endTime_;
	Level currentLevel_;
	Level currentGame_;

	SDL_TimerID gameTimer_;

	GameState currentState_;
	int selectedMenuItem_;

	int gameX_, gameY_;
	int selectedPlayItem_;
	int selectedLevelItem_;
	int selectedHighScoreItem_;
	int highScorePage_;

	class HighScore {
	public:
		HighScore() : moves_(0), elapsedSeconds_(0) {}
		unsigned int moves_;
		time_t elapsedSeconds_;
	};
	HighScore highScore_[LEVEL_COUNT];

};

#endif
