
#include "Input.h"
#include "Graphics.h"
#include <stdlib.h>
#include <map>
#include <string.h>

#ifdef __APPLE__
#include <OpenGL/gl.h> 
#include <GLUT/glut.h>
#else
#include <GL/gl.h> 
#include <GL/glut.h>
#endif

KeyInfo Key;
MouseInfo Mouse;

typedef std::map<int, bool*> KeyMap;
KeyMap keyMap;

void Keyboard(unsigned char key, int x, int y)
{	
	KeyMap::iterator i = keyMap.find((int)key);
	if (i != keyMap.end())
		*keyMap[(int)key] = true;
}

void KeyboardUp(unsigned char key, int x, int y)
{
	KeyMap::iterator i = keyMap.find((int)key);
	if (i != keyMap.end())
		*keyMap[(int)key] = false;
}

void Special(int key, int x, int y)
{
	KeyMap::iterator i = keyMap.find((int)key);
	if (i != keyMap.end())
		*keyMap[key] = true;
}

void SpecialUp(int key, int x, int y)
{
	KeyMap::iterator i = keyMap.find((int)key);
	if (i != keyMap.end())
		*keyMap[key] = false;
}

void MouseClick(int button, int state, int x, int y)
{
	Vector2 lastPos = Mouse.position;
	
	Mouse.position.x = x;
	Mouse.position.y = GetWindowHeight() - y;
	
	Mouse.motion = Mouse.position - lastPos;
	
	switch (button)
	{
		case GLUT_LEFT_BUTTON:
			if (state == GLUT_DOWN)
				Mouse.left = true;
			else if (state == GLUT_UP)
				Mouse.left = false;
			break;
		case GLUT_RIGHT_BUTTON:
			if (state == GLUT_DOWN)
				Mouse.right = true;
			else if (state == GLUT_UP)
				Mouse.right = false;
			break;
		case GLUT_MIDDLE_BUTTON:
			if (state == GLUT_DOWN)
				Mouse.middle = true;
			else if (state == GLUT_UP)
				Mouse.middle = false;
			break;
	}
}

void MouseMove(int x, int y)
{
	Vector2 lastPos = Mouse.position;
	
	Mouse.position.x = x;
	Mouse.position.y = GetWindowHeight() - y;
	
	Mouse.motion = Mouse.position - lastPos;
}

void PassiveMouseMove(int x, int y)
{
	Vector2 lastPos = Mouse.position;
	
	Mouse.position.x = x;
	Mouse.position.y = GetWindowHeight() - y;
	
	Mouse.motion = Mouse.position - lastPos;
}

void InitInput()
{
	glutKeyboardFunc(Keyboard);
	glutKeyboardUpFunc(KeyboardUp);
	glutSpecialFunc(Special);
	glutSpecialUpFunc(SpecialUp);
	glutMouseFunc(MouseClick);
	glutMotionFunc(MouseMove);
	glutPassiveMotionFunc(PassiveMouseMove);
	
	Mouse.position.x = 0;
	Mouse.position.y = 0;
	Mouse.motion.x = 0;
	Mouse.motion.y = 0;
	Mouse.left = false;
	Mouse.right = false;
	Mouse.middle = false;
	
	memset(&Key, 0, sizeof(Key));
	keyMap[GLUT_KEY_UP] = &Key.up;
	keyMap[GLUT_KEY_DOWN] = &Key.down;
	keyMap[GLUT_KEY_LEFT] = &Key.left;
	keyMap[GLUT_KEY_RIGHT] = &Key.right;
	keyMap[13] = &Key.enter;
	keyMap[27] = &Key.escape;
}
