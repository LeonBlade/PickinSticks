#include <stdio.h>

#include "OpenGL.h"
#include "Graphics.h"
#include "Sprite.h"
#include "Log.h"
#include "Map.h"
#include "Camera.h"

#define WIDTH 640
#define HEIGHT 480
#define TILE_SIZE 16

#define MCC(i) i - ((i / TILE_SIZE) * TILE_SIZE)

Graphics *graphics = NULL;
Texture *tilesheet = NULL;
Sprite *cursorSprite = NULL;
Map *map = NULL;
Camera *camera = NULL;

long lastTime = 0;
long holdTime = 0;
int frames = 0;

int xPos = 0;

float zoom = 1.0f;

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


void draw()
{
	// clear color and depth buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	// get time delta
	long curTime = glutGet(GLUT_ELAPSED_TIME);
	long deltaTime = curTime - lastTime;
	holdTime += deltaTime;
	lastTime = curTime;
	frames++;

	if (holdTime >= 1000)
	{
		Log::info("FPS: %i", frames);
		frames = 0;
		holdTime = 0;
	}

	// enable textures
	glEnable(GL_TEXTURE_2D);

	// scale in
	// TODO: make this camera's job
	glScalef(zoom, zoom, 1.0f);

	// render the camera
	camera->draw();

	// draw the cursorSprite
	glColor4f(1.0f, 1.0f, 1.0f, 0.5f);
	cursorSprite->draw();
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

	// draw the map
	map->draw();

	// disable textures
	glDisable(GL_TEXTURE_2D);

	// draw the outline
	glPushMatrix();
	glTranslated(cursorX + 0.01f, cursorY + 0.01f, 0);
	glBegin(GL_LINE_LOOP);
		glVertex2f(0, 0);
		glVertex2f(15.5f, 0);
		glVertex2f(15.5f, 15.5f);
		glVertex2f(0, 15.5f);
	glEnd();
	glPopMatrix();

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
		case 27: 
			exit(0); 
			break;
		case 'n': case 'N':
			map = new Map(tilesheet, WIDTH / TILE_SIZE, HEIGHT / TILE_SIZE);
			break;
		case 'o': case 'O':
			map->loadMap("test.map");
			break;
		case 'p': case 'P':
			map->saveMap("test.map");
			break;
		case 'c': case 'C':
			map->addCrystal(cursorX / TILE_SIZE, cursorY / TILE_SIZE);
			map->compileDL();
			break;
		case 'r': case 'R':
			map->removeCrystal(cursorX / TILE_SIZE, cursorY / TILE_SIZE);
			map->compileDL();
			break;

		case 'w': case 'W':
			if (camera->getY() > 0)
				camera->setPosition(0.0f, -TILE_SIZE, true);
			else
				camera->setPosition(camera->getX(), 0.0f);
			break;
		case 'a': case 'A':
			if (camera->getX() > 0)
				camera->setPosition(-TILE_SIZE, 0.0f, true);
			else
				camera->setPosition(0.0f, camera->getY());
			break;
		case 's': case 'S':
			camera->setPosition(0.0f, TILE_SIZE, true);
			break;
		case 'd': case 'D':
			camera->setPosition(TILE_SIZE, 0.0f, true);
			break;
		case '-':
			camera->setZoom(-1.0f, true, 0.05f);
			break;
		case '+':
			camera->setZoom(1.0f, true, 0.05f);
			break;
		case '/': camera->setRotation(-1.0f, true, 0.05f); break;
		case '*': camera->setRotation(1.0f, true, 0.05f); break;
		default:
			Log::info("Key %c ~ %i", key, key);
			break;
	}
}

void ukey(unsigned char key, int x, int y)
{
	
}

void skey(int key, int x, int y)
{
	printf("key %i\n", key);
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
 * unProject is called when you want to map X and Y based on screen scaling
 */
void handleMouse(int x, int y)
{
	GLint viewport[4];
	GLdouble projection[16];
	GLdouble modelview[16];
	
	glGetIntegerv(GL_VIEWPORT, viewport);
	glGetDoublev(GL_PROJECTION_MATRIX, projection);
	glGetDoublev(GL_MODELVIEW_MATRIX, modelview);

	int xx = x;
	int yy = viewport[3] - y;
	GLdouble _x, _y, _z;
	gluUnProject(xx, yy, 0, modelview, projection, viewport, &_x, &_y, &_z);

	x = (int)_x;
	y = (int)_y;

	int cx = camera->getX();
	int cy = camera->getY();

	int mx = (x - (x % TILE_SIZE)) + MCC(cx);
	int my = (y - (y % TILE_SIZE)) + MCC(cy);

	cursorX = mx;
	cursorY = my;
}

/**
 * dmouse is called whenever the mouse is moving and one or more buttons are clicked
 */
void dmouse(int x, int y)
{
	// handle mouse stuff
	handleMouse(x, y);	

	if (buttonState.left)
	{
		uint8_t tile = XY2I(tileX, tileY, tilesheet->width);
		printf("tile %i\n", tile);
		map->setTile(tile, cursorX, cursorY);
		map->compileDL();
	}
}

/**
 * umouse is called whenever the mouse is moving but no buttons are down
 */
void umouse(int x, int y)
{
	// handle mouse stuff
	handleMouse(x, y);	

	cursorSprite->setPosition(cursorX, cursorY);
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

	map = new Map(tilesheet, 20, 15);
	map->loadMap("test.map");

	camera = new Camera();

	Log::info("Initialization complete!");

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
	// center the window
	glutInitWindowPosition((glutGet(GLUT_SCREEN_WIDTH)-WIDTH)/2, (glutGet(GLUT_SCREEN_HEIGHT)-HEIGHT)/2);
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

	// enable things we need
	glEnable(GL_TEXTURE_2D);
	// glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);

	// enable transparency
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// set line width
	glLineWidth(1.0f);

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
