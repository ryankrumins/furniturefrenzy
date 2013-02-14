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

#include <SDL.h>

// 0x7E - 0x20
SDL_Rect fontRects[0x5E] = {
	{  0,  0, 18, 29}, //
	{ 24,  0,  9, 29}, // !
	{ 39,  0, 11, 29}, // "
	{ 56,  0, 16, 29}, // #
	{ 78,  0, 14, 29}, // $
	{ 98,  0, 18, 29}, // %
	{122,  0, 16, 29}, // &
	{144,  0,  7, 29}, // '
	{157,  0, 10, 29}, // (
	{173,  0,  9, 29}, // )
	{188,  0,  9, 29}, // *
	{203,  0, 14, 29}, // +
	{223,  0,  6, 29}, // ,
	{235,  0,  9, 29}, // -
	{250,  0,  7, 29}, // ,
	{263,  0, 16, 29}, // /

	{  0, 35, 14, 29}, // 0
	{ 20, 35, 12, 29}, // 1
	{ 38, 35, 14, 29}, // 2
	{ 58, 35, 14, 29}, // 3
	{ 78, 35, 15, 29}, // 4
	{ 99, 35, 14, 29}, // 5
	{119, 35, 14, 29}, // 6
	{139, 35, 12, 29}, // 7
	{157, 35, 14, 29}, // 8
	{177, 35, 14, 29}, // 9
	{197, 35,  7, 29}, // :
	{210, 35,  7, 29}, // ;
	{223, 35, 14, 29}, // <
	{243, 35, 14, 29}, // =
	{263, 35, 14, 29}, // >
	{283, 35, 14, 29}, // ?

	{  0, 70, 20, 29}, // @
	{ 26, 70, 15, 29}, // A
	{ 47, 70, 14, 29}, // B
	{ 67, 70, 14, 29}, // C
	{ 87, 70, 14, 29}, // D
	{107, 70, 12, 29}, // E
	{125, 70, 12, 29}, // F
	{143, 70, 14, 29}, // G
	{163, 70, 14, 29}, // H
	{183, 70,  8, 29}, // I
	{197, 70, 10, 29}, // J
	{213, 70, 15, 29}, // K
	{234, 70, 11, 29}, // L
	{251, 70, 18, 29}, // M
	{275, 70, 14, 29}, // N
	{295, 70, 14, 29}, // O

	{  0,105, 14, 29}, // P
	{ 20,105, 14, 29}, // Q
	{ 40,105, 14, 29}, // R
	{ 60,105, 14, 29}, // S
	{ 80,105, 14, 29}, // T
	{100,105, 14, 29}, // U
	{120,105, 16, 29}, // V
	{142,105, 22, 29}, // W
	{170,105, 15, 29}, // X
	{191,105, 15, 29}, // Y
	{212,105, 13, 29}, // Z
	{231,105,  9, 29}, // [
	{246,105, 13, 29}, // '\'
	{265,105,  8, 29}, // ]
	{279,105, 15, 29}, // ^
	{300,105, 16, 29}, // _

	{  0,140, 10, 29}, // `
	{ 16,140, 13, 29}, // a
	{ 35,140, 14, 29}, // b
	{ 55,140, 13, 29}, // c
	{ 74,140, 14, 29}, // d
	{ 94,140, 14, 29}, // e
	{114,140, 10, 29}, // f
	{130,140, 14, 29}, // g
	{150,140, 14, 29}, // h
	{170,140,  8, 29}, // i
	{184,140,  9, 29}, // j
	{199,140, 14, 29}, // k
	{219,140,  8, 29}, // l
	{233,140, 19, 29}, // m
	{258,140, 14, 29}, // n
	{278,140, 14, 29}, // o

	{  0,175, 14, 29}, // p
	{ 20,175, 14, 29}, // q
	{ 40,175, 11, 29}, // r
	{ 57,175, 13, 29}, // s
	{ 76,175, 11, 29}, // t
	{ 93,175, 14, 29}, // u
	{113,175, 14, 29}, // v
	{133,175, 19, 29}, // w
	{158,175, 14, 29}, // x
	{178,175, 14, 29}, // y
	{198,175, 12, 29}, // z
	{216,175, 11, 29}, // {
	{233,175,  6, 29}, // |
	{245,175, 11, 29}, // }
};

