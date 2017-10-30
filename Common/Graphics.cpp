
#include "Graphics.h"
#include "Timer.h"
#include "Input.h"
#include "Vector3.h"

#ifdef __APPLE__
#include <OpenGL/gl.h> 
#include <GLUT/glut.h>
#else
#include <GL/gl.h> 
#include <GL/glut.h>
#endif

Fonts Font = 
{
	GLUT_BITMAP_8_BY_13,
	GLUT_BITMAP_HELVETICA_10,
	GLUT_BITMAP_TIMES_ROMAN_10
};

UpdateFunc UpdateCallback;
RenderFunc RenderCallback;

Timer frameTimer;

enum RenderMode { RENDER3D, RENDER2D };
typedef enum RenderMode RenderMode;
RenderMode renderMode = RENDER3D;

void DisplayEvent()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	RenderCallback();
	glutSwapBuffers();
}

void UpdateEvent(int fps)
{
	glutTimerFunc(1000 / fps, UpdateEvent, fps);
	UpdateCallback(frameTimer.ElapsedSeconds());
	frameTimer.Reset();
	DisplayEvent();
	
	Mouse.motion.x = 0;
	Mouse.motion.y = 0;
}

void InitWindow(int width, int height, const std::string &caption, const Colour &colour)
{
	int argc = 0;
	char **argv = NULL;
	
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(width, height);
	glutCreateWindow(caption.c_str());
	
	glClearColor(colour.r, colour.g, colour.b, 1);
	
	glViewport(0, 0, width, height);
	OrthographicMode();
	
	glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
	
	InitInput();
}

void RunApp(int fps, UpdateFunc update, RenderFunc render)
{
	UpdateCallback = update;
	RenderCallback = render;
	
	glutDisplayFunc(DisplayEvent);
	glutTimerFunc(1000 / fps, UpdateEvent, fps);
	frameTimer.Start();
	glutMainLoop();
}

int GetWindowWidth()
{
	return glutGet(GLUT_WINDOW_WIDTH);
}

int GetWindowHeight()
{
	return glutGet(GLUT_WINDOW_HEIGHT);
}

void PerspectiveMode(Camera *cam)
{
	renderMode = RENDER3D;
	
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf(cam->GetProjectionMatrix().ToArray());
	
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(cam->GetViewMatrix().ToArray());
}

void OrthographicMode() 
{
	renderMode = RENDER2D;
	
    glMatrixMode(GL_PROJECTION);
	Matrix4x4 projection;
	projection.SetProjectionOrthographic(GetWindowWidth(), GetWindowHeight());
	glLoadMatrixf(projection.ToArray());
	
	glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void RenderRectangle(int x, int y, int w, int h, const Colour &colour)
{
	glColor3fv(colour.ToArray());
	glBegin(GL_QUADS);
	glVertex2s(x, y);
	glVertex2s(x + w, y);
	glVertex2s(x + w, y + h);
	glVertex2s(x, y + h);
	glEnd();
}

void RenderCube(int x, int y, int z, int size, const Colour &colour)
{
	glColor3fv(colour.ToArray());
	glPushMatrix();
		glTranslated(x, y, z);
		glutSolidCube(size);
	glPopMatrix();
}

void RenderText(int x, int y, void *font, const std::string &text, const Colour &colour) 
{ 
	glColor3fv(colour.ToArray());
    glRasterPos2f((float)x, (float)y);
	int i;
    for (i = 0; i < text.length(); i++)
        glutBitmapCharacter(font, text[i]);
}

void SetColour(float hue) 
{
	glColor3fv(Colour::FromHSV(hue * 360.0f, 1.0f, 1.0f).ToArray());
}

void RenderHeightmap(const Heightmap &hmap, const Vector3 &scale, float min, float max)
{
	glPushMatrix();
	glScalef(scale.x, scale.y, scale.z);
	
	glColor3fv(Colour::Black().ToArray());
	glBegin(GL_QUADS);
	for (int x = 0; x < hmap.GetWidth() - 1; x++)
	{
		for (int z = 0; z < hmap.GetDepth() - 1; z++)
		{
			glVertex3f(x, hmap(x, z + 1), z + 1);
			glVertex3f(x + 1, hmap(x + 1, z + 1), z + 1);
			glVertex3f(x + 1, hmap(x + 1, z), z);
			glVertex3f(x, hmap(x, z), z);
		}
	}
	glEnd();

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glBegin(GL_QUADS);
	for (int x = 0; x < hmap.GetWidth() - 1; x++)
	{
		for (int z = 0; z < hmap.GetDepth() - 1; z++)
		{
			SetColour((hmap(x, z + 1) - min) / max);
			glVertex3f(x, hmap(x, z + 1), z + 1);
			
			SetColour((hmap(x + 1, z + 1) - min) / max);
			glVertex3f(x + 1, hmap(x + 1, z + 1), z + 1);
			
			SetColour((hmap(x + 1, z) - min) / max);
			glVertex3f(x + 1, hmap(x + 1, z), z);
			
			SetColour((hmap(x, z) - min) / max);
			glVertex3f(x, hmap(x, z), z);
		}
	}
	glEnd();
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	
	glPopMatrix();
}
