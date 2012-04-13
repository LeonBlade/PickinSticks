#ifndef __GRAPHICS_H__
#define __GRAPHICS_H__

#include "OpenGL.h"

#include <vector>

/**
 * Texture struct contains information about a texture and holds the texture GLuint
 */
struct Texture
{
	Texture();
	~Texture();

	GLuint texture;
	int width;
	int height;
};

/**
 * Graphics class handles anything about loading in different graphics
 */
class Graphics
{

public:
	Graphics();
	~Graphics();

	Texture *loadTexture(const char *filename); // load in a texture into memory and return it
	Texture *getTexture(int index); // grabs a texture from the pile and returns it
	void deleteTexture(int index); // delete texture from memory

private:
	std::vector<Texture*> texturePile; // the pile of textures we have allocated

};

#endif
