#include "Sprite.h"

#include <math.h>

Sprite::Sprite(Texture *texture, int width, int height)
{
	// store the current texture
	this->texture = texture;

	// set X and Y to 0 by default
	this->x = 0;
	this->y = 0;

	// set the width and height
	this->w = width;
	this->h = height;

	// set the frame to 0 by default
	this->frame = 0;
}

Sprite::~Sprite()
{

}

void Sprite::draw()
{
	int tw = this->texture->width / this->w;

	int fx = this->frame % tw;
	int fy = floor(this->frame / tw);

	glPushMatrix();

	glTranslatef(this->x, this->y, 0.0f);

	float min_x = (float) (fx * this->w) / this->texture->width;
	float max_y = (float) (this->texture->height - (fy * this->h)) / this->texture->height;
	float max_x = (float) ((fx * this->w) + this->w) / this->texture->width;
	float min_y = (float) (this->texture->height - ((fy * this->h) + this->h)) / this->texture->height;

	glBindTexture(GL_TEXTURE_2D, this->texture->texture);

	glBegin(GL_QUADS); 
		glTexCoord2f(min_x, max_y);
		glVertex2f(0, 0);
		glTexCoord2f(max_x, max_y);
		glVertex2f(this->w, 0);
		glTexCoord2f(max_x, min_y);
		glVertex2f(this->w, this->h);
		glTexCoord2f(min_x, min_y);
		glVertex2f(0, this->h);
	glEnd();

	glPopMatrix();
}

void Sprite::setPosition(int x, int y)
{
	this->x = x;
	this->y = y;
}

void Sprite::setSize(int w, int h)
{
	this->w = w;
	this->h = h;
}

void Sprite::setFrame(int index)
{
	this->frame = index;
}

void Sprite::setFrame(int x, int y)
{
	int tw = this->texture->width / this->w;

	this->frame = (((x+1)*(y+1))+((tw-(x+1))*y)) - 1;
}
