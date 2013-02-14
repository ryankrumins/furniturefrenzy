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

#include "Level.h"

#include <algorithm>
#include <iostream>

Level::Level(const char *data, unsigned int x, unsigned int y,
		unsigned int exitY) :
	x_(x), y_(y), exit_(exitY), complete_(false)
{
	unsigned int upper = x*y;

	levelData_.resize(upper);
	for (unsigned int index = 0; index < upper; ++index) {
		levelData_[index] = (Obstacle)data[index];	
	}
	occupiedMask_.resize(upper);

	updateOccupiedMask();
}

void Level::updateOccupiedMask()
{
	unsigned int upper = x_*y_;

	std::fill(occupiedMask_.begin(), occupiedMask_.end(), 0);
	
	for (unsigned int index = 0; index < upper; ++index) {
		Obstacle obs = levelData_[index];

		if (obs == Empty) {
			continue;
		}

		Orientation orientation = obstacleOrientation(obs);
		unsigned int osize = obstacleSize(obs);
		unsigned int multiplier = orientation == Horizontal ? 1 : x_;

		for (unsigned int offs = 0; offs < osize; ++offs) {
				unsigned int delta = offs*multiplier;
				occupiedMask_[index+delta] = delta;
		}
	}
}

void Level::move(unsigned int x, unsigned int y, Direction dir)
{
	int index = x+y*x_;

	index -= occupiedMask_[index];
	Obstacle obs = levelData_[index];

	x = index % x_;
	y = index / y_;

	// check for level completion
	if (obs == BlueTable && x == x_-2 && y == exit_ && dir == Right) {
		complete_ = true;
		return;
	}

	Orientation orientation = obstacleOrientation(obs);
	int multiplier = (orientation == Horizontal ? 1 : (int)x_);
	
	if (dir == Up || dir == Left)
		multiplier = -multiplier;

	levelData_[index] = Empty;
	levelData_[index+multiplier] = obs;

	updateOccupiedMask();
}

bool Level::canMove(unsigned int x, unsigned int y, Direction dir) const
{
	int index = x+y*x_;
	
	index -= occupiedMask_[index];
	Obstacle obs = levelData_[index];

	x = index % x_;
	y = index / y_;

	if (obs == Empty) {
		return false;
	}

	Orientation orientation = obstacleOrientation(obs);
	unsigned int osize = obstacleSize(obs);
	unsigned int osizepos = (dir == Up || dir == Left) ? 1 : osize;
	int multiplier = (orientation == Horizontal ? 1 : (int)x_);
	
	if (dir == Up || dir == Left)
		multiplier = -multiplier;

	// invalidate impossible directions
	if (orientation == Horizontal && (dir == Up || dir == Down)) {
		return false;
	}
	if (orientation == Vertical && (dir == Left || dir == Right)) {
		return false;
	}

	// check for level completion
	if (obs == BlueTable && x == x_-2 && y == exit_ && dir == Right) {
		return true;
	}

	// check for boundary conditions
	if (orientation == Horizontal) {
		if (x == 0 && dir == Left) {
			return false;
		}
		if (x+osize >= x_ && dir == Right) {
			return false;
		}
	} else {
		if (y == 0 && dir == Up) {
			return false;
		}
		if (y+osize >= y_ && dir == Down) {
			return false;
		}
	}

	// check occupied mask
	int occupiedIndex = index+((osizepos)*multiplier);
	//int occupiedIndex = index+multiplier;
	int occupiedOffset = occupiedMask_[occupiedIndex];
	if (occupiedOffset > 0 && (occupiedIndex-occupiedOffset) != index) {
		return false;
	}

	// check level data
	if (levelData_[occupiedIndex] != Empty) {
		return false;
	}

	return true;
}

Level::Orientation Level::obstacleOrientation(Obstacle obs)
{
	switch (obs) {
		case Empty:
			return Horizontal;
		case BlueTable:
		case OfficeDesk:
		case BookShelf:
			return Horizontal;
		case Piano:
		case Sofa:
			return Vertical;
	}
	return Horizontal;
}

unsigned int Level::obstacleSize(Obstacle obs)
{
	switch (obs) {
		case Empty:
			return 0;
		case BlueTable:
		case OfficeDesk:
		case Piano:
			return 2;
		case Sofa:
		case BookShelf:
			return 3;
	}
	return 0;
}
