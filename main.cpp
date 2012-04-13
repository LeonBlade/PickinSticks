#include <stdio.h>

#include "OpenGL.h"
#include "Graphics.h"
#include "Log.h"

#define WIDTH 800
#define HEIGHT 600

Graphics *graphics;

void draw()
{
	// clear color and depth buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	// swap buffers
	glutSwapBuffers();
}

void idle()
{
	glutPostRedisplay();
}

/**
 * init function initializes everything for the game
 */
bool init()
{
	// initialize graphics	
	graphics = new Graphics();

	// load in our texture
	Texture *texture = graphics->loadTexture("resources/tilesheet.png");

	// make sure the texture loaded
	if (texture == NULL)
		return false;

	return true;
}

int main(int argc, char *argv[])
{
	// initialize GLUT
	glutInit(&argc, argv);
	// set display mode enabling depth double buffer and rgba
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	// set the window size
	glutInitWindowSize(WIDTH, HEIGHT);
	// create window with title
	glutCreateWindow("Pickin' Sticks");

	// register callbacks
	glutDisplayFunc(draw);
	//glutReshapeFunc(resize);
	glutIdleFunc(idle);
	//glutKeyboardFunc(keys);
	//glutKeyboardUpFunc(ukey);
	//glutSpecialFunc(skey);
	//glutSpecialUpFunc(uskey);

	// call initialize 
	if (!init())
	{
		Log::error("Initialize failed!");
		return 1;
	}

	// set clear color to black
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	// main glut loop
	glutMainLoop();

	return 0;
}
