#include "Graphics.h"
#include "Log.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <libpng/png.h>

Graphics::Graphics()
{
	Log::info("Graphics was initialized.");
}

/**
 * loadTexture takes a filename and returns a Texture pointer to the texture place in memory
 */
Texture *Graphics::loadTexture(const char *filename)
{
	// log it out
	Log::info("Loading texture \"%s\"...", filename);

	//header for testing if it is a png
	png_byte header[8];

	//open file as binary
	FILE *fp = fopen(filename, "rb");
	if (!fp)
	{	
		Log::error("File failed to load!");
		return NULL;
	}

	//read the header
	fread(header, 1, 8, fp);

	//test if png
	int is_png = !png_sig_cmp(header, 0, 8);
	if (!is_png) 
	{
		Log::error("Image is not a PNG!");
		fclose(fp);
		return NULL;
	}

	//create png struct
	png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if (!png_ptr) 
	{
		Log::error("png_ptr failed to create read struct!");
		fclose(fp);
		return NULL;
	}

	//create png info struct
	png_infop info_ptr = png_create_info_struct(png_ptr);
	if (!info_ptr) 
	{
		Log::error("info_ptr failed to create struct!");
		png_destroy_read_struct(&png_ptr, (png_infopp) NULL, (png_infopp) NULL);
		fclose(fp);
		return NULL;
	}

	//create png info struct
	png_infop end_info = png_create_info_struct(png_ptr);
	if (!end_info) 
	{
		Log::error("end_info failed to create struct!");
		png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp) NULL);
		fclose(fp);
		return NULL;
	}

	//png error stuff, not sure libpng man suggests this.
	if (setjmp(png_jmpbuf(png_ptr))) 
	{
		Log::error("png_jumpbuf overflow!");
		png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
		fclose(fp);
		return NULL;
	}

	//init png reading
	png_init_io(png_ptr, fp);

	//let libpng know you already read the first 8 bytes
	png_set_sig_bytes(png_ptr, 8);

	// read all the info up to the image data
	png_read_info(png_ptr, info_ptr);

	//variables to pass to get info
	int bit_depth, color_type;
	png_uint_32 twidth, theight;

	// get info about png
	png_get_IHDR(png_ptr, info_ptr, &twidth, &theight, &bit_depth, &color_type, NULL, NULL, NULL);

	//update width and height based on png info
	int width = twidth;
	int height = theight;

	// Update the png info struct.
	png_read_update_info(png_ptr, info_ptr);

	// Row size in bytes.
	int rowbytes = png_get_rowbytes(png_ptr, info_ptr);

	// Allocate the image_data as a big block, to be given to opengl
	png_byte *image_data = new png_byte[rowbytes * height];
	if (!image_data) 
	{
		Log::error("image_data failed to create a new png_byte!");
		//clean up memory and close stuff
		png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
		fclose(fp);
		return NULL;
	}

	//row_pointers is for pointing to image_data for reading the png with libpng
	png_bytep *row_pointers = new png_bytep[height];
	if (!row_pointers) 
	{
		Log::error("row_pointers failed to create a new png_bytep!");
		//clean up memory and close stuff
		png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
		delete[] image_data;
		fclose(fp);
		return NULL;
	}

	// set the individual row_pointers to point at the correct offsets of image_data
	for (int i = 0; i < height; ++i)
		row_pointers[height - 1 - i] = image_data + i * rowbytes;

	//read the png into image_data through row_pointers
	png_read_image(png_ptr, row_pointers);

	//Now generate the OpenGL texture object
	GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_TEXTURE_MIN_FILTER);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D,0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, (GLvoid*) image_data);
	// gluBuild2DMipmaps(GL_TEXTURE_2D, 4, width, height, GL_RGBA, GL_UNSIGNED_BYTE, image_data);

	//clean up memory and close stuff
	png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
	delete[] image_data;
	delete[] row_pointers;
	fclose(fp);

	// create the texture struct
	Texture *_texture = new Texture;
	_texture->texture = texture;
	_texture->width = width;
	_texture->height = height;

	// store the texture in the vector
	this->texturePile.push_back(_texture);

	// log the texture being loaded
	Log::info("Loaded texture \"%s\" at %i...", filename, _texture->texture);

	// return the texture
	return this->texturePile.at(texture - 1);
}

/**
 * getTexture returns a texture pointer
 */
Texture *Graphics::getTexture(int index)
{
	return this->texturePile.at(index);
}

/**
 * deleteTexture deletes a texture from memory
 */
void Graphics::deleteTexture(int index)
{
	this->texturePile.erase(this->texturePile.begin() + index);
}

/**
 * Texture contstructor will intialize the variables
 */
Texture::Texture()
{
	texture = 0;
	width = 0;
	height = 0;
}

/**
 * ~Texture destructor will delete the texture from OpenGL
 */
Texture::~Texture()
{
	// log destruction of texture
	Log::info("Texture %i being destructed...", texture);
	// delete the texture
	glDeleteTextures(1, &texture);
}


