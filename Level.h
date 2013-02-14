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

#ifndef _LEVEL_H_
#define _LEVEL_H_

#include <vector>

class Level
{
public:
	typedef enum {Left, Right, Up, Down} Direction;
	typedef enum {Vertical, Horizontal} Orientation;
	typedef enum {Empty, BlueTable, OfficeDesk, Piano, Sofa,
			BookShelf} Obstacle;

	static Orientation obstacleOrientation(Obstacle obs);
	static unsigned int obstacleSize(Obstacle obs);

	Level(const char *data, unsigned int x, unsigned int y, unsigned int exitY);

	bool canMove(unsigned int x, unsigned int y, Direction dir) const;
	// move assumes canMove has been checked and is true
	void move(unsigned int x, unsigned int y, Direction dir);

	inline bool completed() const { return complete_; }

	std::vector<Obstacle> levelData_;
	std::vector<unsigned int> occupiedMask_;
	unsigned int x_;
	unsigned int y_;
	unsigned int exit_;
	bool complete_;

protected:
	void updateOccupiedMask();

};

#endif

