
/*
 * @file	Graphics.h/.cpp
 * @brief	Abstracts rendering related tasks from OpenGL API.
 * @details	Provides functions for rendering rectangles, cubes, text, and heightmaps.
 *			Rendering functions should be used in RenderFunc event supplied to RunApp() call.
 *			Update event will be triggered roughly every 1/FPS seconds. DeltaTime parameter
 *			should be used for scaling to ensure framerate independance.
 *			OrthographicMode() should be called before rendering text or rectangles.
 *			PerspectiveMode(cam) should be called before rendering cubes or heightmaps.
 *			Render mode defaults to orthographic.
 * @author	Matt Drage
 * @date	04/11/2012
 */

#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <string>
#include "Camera.h"
#include "Heightmap.h"
#include "Colour.h"

// Built-in GLUT fonts
struct Fonts
{
	void *fixed;
	void *helvetica;
	void *timesNewRoman;
};
typedef struct Fonts Fonts;
extern Fonts Font;

// Function callback types for update and render events
typedef void (*UpdateFunc)(double);
typedef void (*RenderFunc)(void);

// Initialization
void InitWindow(int width, int height, const std::string &caption, const Colour &colour);
void RunApp(int fps, UpdateFunc update, RenderFunc render);

// Window dimensions
int GetWindowWidth();
int GetWindowHeight();

// Set rendering mode
void PerspectiveMode(Camera *cam);
void OrthographicMode();

// Render primitives/objects/text
void RenderRectangle(int x, int y, int w, int h, const Colour &colour);
void RenderCube(int x, int y, int z, int size, const Colour &colour);
void RenderText(int x, int y, void *font, const std::string &text, const Colour &colour);
void RenderHeightmap(const Heightmap &hmap, const Vector3 &scale, float min, float max);

#endif
