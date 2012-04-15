#ifndef __MAP_H__
#define __MAP_H__

#include "Graphics.h"
#include "Sprite.h"

#include <map>

struct MapHeader
{
	MapHeader(int w, int h); // header constructor

	char magic[4]; // make sure the file is actually a map file
	int filesize; // used to make sure the file doesn't have an error
	int width; // the width of the map
	int height; // the height of the map
};

struct MapData
{
	MapData(MapHeader *header);
	~MapData();

	uint8_t *tiles;
	uint8_t crystalCount;
	int16_t *crystals;
};

class Map
{

public:
	Map(Texture *texture, int width = 20, int height = 15);
	~Map();

	bool loadMap(const char *filename);
	bool saveMap(const char *filename);

	void setTile(char tile, int x, int y);
	char getTile(int x, int y);

	void addCrystal(int x, int y);
	void removeCrystal(int x, int y);

	void draw();
	void compileDL();

private:
	MapHeader *header;
	MapData *data;
	Texture *texture;
	Sprite *drawSprite;
	GLuint displayList;

};

#endif
