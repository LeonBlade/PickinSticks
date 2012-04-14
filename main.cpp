#include <stdio.h>

#include "OpenGL.h"
#include "Graphics.h"
#include "Sprite.h"
#include "Log.h"

#define WIDTH 640
#define HEIGHT 480

Graphics *graphics = NULL;
Texture *tilesheet = NULL;
Sprite *cursorSprite = NULL;

struct Button
{
	bool left;
	bool middle;
	bool right;
};

Button buttonState;

int cursorX = 0;
int cursorY = 0;

int tileX = 0;
int tileY = 0;

int map[WIDTH / 16][HEIGHT / 16];

void draw()
{
	// clear color and depth buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	// enable textures
	glEnable(GL_TEXTURE_2D);
	glDisable(GL_DEPTH_TEST);

	Sprite *tempSprite = new Sprite(tilesheet, 16, 16);

	for (int y = 0; y < HEIGHT / 16; y++)
	{
		for (int x = 0; x < WIDTH / 16; x++)
		{
			tempSprite->setPosition(x * 16, y * 16);
			tempSprite->setFrame(map[x][y]);
			tempSprite->draw();
		}
	}

	delete tempSprite;

	// draw the cursorSprite
	glColor4f(1.0f, 1.0f, 1.0f, 0.5f);
	cursorSprite->draw();
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

	// disable textures
	glDisable(GL_TEXTURE_2D);

	// draw the outline
	glPushMatrix();
	glTranslated(cursorX, cursorY, 0);
	glBegin(GL_LINE_LOOP);
		glVertex2i(16, 0);
		glVertex2i(0, 0);
		glVertex2i(0, 16);
		glVertex2i(16, 16);
	glEnd();
	glPopMatrix();

	glEnable(GL_DEPTH_TEST);

	// swap buffers
	glutSwapBuffers();
}

void idle()
{
	glutPostRedisplay();
}

void resize(int w, int h)
{
	// just in case height is 0 fix it
	if(h == 0) h = 1;

	// switch to projection
	glMatrixMode(GL_PROJECTION);
	// reset matrix
	glLoadIdentity();

	// set the viewport
	glViewport(0, 0, w, h);
	// set the ortho for the scene
	glOrtho(0.0f, w, h, 0.0f, -1.0f, 1.0f);

	// go back to modelview
	glMatrixMode(GL_MODELVIEW);
}

void keys(unsigned char key, int x, int y)
{
	switch(key)
	{
		case 27: exit(0); break;
		case 's': case 'S':
			FILE *file = fopen("test.map", "wb");
			if (!file)
			{
				Log::error("Couldn't open \"test.map\" for writing!");
				exit(0);
			}
			fwrite(map, 1, sizeof(map), file);
			fclose(file);
		break;
	}
}

void ukey(unsigned char key, int x, int y)
{
	
}

void skey(int key, int x, int y)
{
	switch (key)
	{
		case GLUT_KEY_LEFT: tileX--; break;
		case GLUT_KEY_RIGHT: tileX++; break;
		case GLUT_KEY_UP: tileY--; break;
		case GLUT_KEY_DOWN: tileY++; break;
	}

	cursorSprite->setFrame(tileX, tileY);
}

void uskey(int key, int x, int y)
{
	
}

/**
 * dmouse is called whenever the mouse is moving and one or more buttons are clicked
 */
void dmouse(int x, int y)
{
	int mx = (x - (x % 16));
	int my = (y - (y % 16));

	cursorX = mx;
	cursorY = my;

	if (buttonState.left)
	{
		int mx = cursorX / 16;
		int my = cursorY / 16;

		map[mx][my] = (((tileX+1)*(tileY+1))+(((tilesheet->width/16)-(tileX+1))*tileY)) - 1;
	}
}

/**
 * umouse is called whenever the mouse is moving but no buttons are down
 */
void umouse(int x, int y)
{
	int mx = (x - (x % 16));
	int my = (y - (y % 16));

	cursorX = mx;
	cursorY = my;

	cursorSprite->setPosition(mx, my);
}

/**
 * mouse is called whenever the mouse is clicked
 */
void mouse(int button, int state, int x, int y)
{
	switch (button)
	{
		case GLUT_LEFT_BUTTON: buttonState.left = (state == GLUT_DOWN) ? true : false; break;
		case GLUT_MIDDLE_BUTTON: buttonState.middle = (state == GLUT_DOWN) ? true : false; break;
		case GLUT_RIGHT_BUTTON: buttonState.right = (state == GLUT_DOWN) ? true : false; break;
	}

	dmouse(x, y);
}

/**
 * init function initializes everything for the game
 */
bool init()
{
	// initialize graphics	
	graphics = new Graphics();

	// load in our texture
	tilesheet = graphics->loadTexture("resources/tilesheet.png");

	// make sure the texture loaded
	if (tilesheet == NULL)
		return false;

	cursorSprite = new Sprite(tilesheet, 16, 16);
	cursorSprite->setFrame(0);

	// load the map
	FILE *file = fopen("test.map", "rb");
	if (file == NULL)
	{
		for (int y = 0; y < HEIGHT / 16; y++)
		{
			for (int x = 0; x < WIDTH / 16; x++)
			{
				map[x][y] = 0;
			}
		}
	}
	else
	{
		fseek(file, 0, SEEK_END);
  		long size = ftell(file);
  		rewind(file);
		fread(map, 1, size, file);;
	}
	fclose(file);

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
	glutReshapeFunc(resize);
	glutIdleFunc(idle);
	glutKeyboardFunc(keys);
	glutKeyboardUpFunc(ukey);
	glutSpecialFunc(skey);
	glutSpecialUpFunc(uskey);
	glutMouseFunc(mouse);
	glutMotionFunc(dmouse);
	glutPassiveMotionFunc(umouse);

	glEnable(GL_TEXTURE_2D);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glLineWidth(2.0f);

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
