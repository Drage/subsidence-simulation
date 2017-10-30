
/*
 * @file	Input.h/.cpp
 * @brief	Manages keyboard and mouse input.
 * @details	Abstracts obtainment of input from GLUT API.
 *			Initialisation is handled automatically when application window is created.
 * @author	Matt Drage
 * @date	20/12/2012
 */

#ifndef INPUT_H
#define INPUT_H

#include "Vector2.h"

// Current keyboard state
struct KeyInfo
{
	bool up;
	bool down;
	bool left;
	bool right;
	bool enter;
	bool escape;
};
typedef struct KeyInfo KeyInfo;
extern KeyInfo Key;

// Current mouse state
struct MouseInfo
{
	Vector2 position;
	Vector2 motion;
	bool left;
	bool right;
	bool middle;
};
typedef struct MouseInfo MouseInfo;
extern MouseInfo Mouse;

void InitInput();

#endif
