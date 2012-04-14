#ifndef __SPRITE_H__
#define __SPRITE_H__

#include "Graphics.h"
#include "Log.h"

class Sprite
{

public:
	Sprite(Texture *texture, int width = 16, int height = 16);
	~Sprite(); // delete the sprite

	void setPosition(int x, int y);
	void setSize(int w, int h);
	void setFrame(int index);
	void setFrame(int x, int y);

	void draw();

private:
	Texture *texture; // pointer to the texture for this sprite
	int x, y; // the X and Y coordinates of the sprite
	int w, h; // the width and height of the sprite
	int frame; // the frame for which part of the sprite we're looking at
};

#endif
