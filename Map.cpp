#include "Map.h"

#include "Log.h"

#include <stdio.h>

Map::Map(Texture *texture, int width, int height)
{
	this->texture = texture;
	this->drawSprite = new Sprite(this->texture);

	this->header = new MapHeader(width, height);
	this->data = new MapData(this->header);

	this->displayList = glGenLists(1);

	this->compileDL();
}

Map::~Map()
{
	delete this->header;
	delete this->data;
}

bool Map::loadMap(const char *filename)
{
	// open a file handle to the file
	FILE *file = fopen(filename, "rb");

	// if the file pointer is null then we return false
	if (file == NULL)
	{
		Log::error("Failed to load map \"%s\"!", filename);
		return false;
	}

	// read the header data
	fread(this->header, 1, sizeof(MapHeader), file);
	// create the map data which will allocate memory for it
	this->data = new MapData(this->header);
	// store the tile data size
	int tileSize = this->header->width * this->header->height;
	// read the tile data
	fread(this->data->tiles, 1, tileSize, file);
	// read crystal count
	fread(&this->data->crystalCount, 1, 1, file);
	// read in the crystal data
	fread(this->data->crystals, 1, sizeof(MapCrystal) * this->data->crystalCount, file);

	// close the file handle
	fclose(file);

	// compile the display list to draw
	this->compileDL();

	// log the successful map load
	Log::info("Loaded map file \"%s\"...", filename);

	// map loaded successfully
	return true;
}

bool Map::saveMap(const char *filename)
{
	// open a file handle to the file
	FILE *file = fopen(filename, "wb");
	// if the file handle is null then return false
	if (file == NULL)
	{
		Log::error("Failed to save map \"%s\"!", filename);
		return false;
	}

	// get the crystals
	int crystalCount = this->crystalPile.size();
	this->data->crystals = (MapCrystal*)malloc(sizeof(MapCrystal) * crystalCount);	
	std::map<int, MapCrystal*>::iterator it;

	// create a temp holder for crystals
	MapCrystal *tCrystals[crystalCount];

	// loop through the crystals in the pile and throw them in this array
	int i = 0;
	for (it = this->crystalPile.begin(); it != this->crystalPile.end(); it++, i++)
		tCrystals[i] = (MapCrystal*)it->second;

	// put the array into the crystals
	this->data->crystals = *tCrystals;

	fwrite(this->header, 1, sizeof(MapHeader), file);
	fwrite(this->data->tiles, 1, this->header->width * this->header->height, file);
	fwrite(&this->data->crystalCount, 1, 1, file);
	fwrite(this->data->crystals, 1, sizeof(MapCrystal) * crystalCount, file);

	fclose(file);

	// log the successful map save
	Log::info("Saved map file \"%s\"...", filename);

	// map saved successfully
	return true;
}

void Map::draw()
{
	glCallList(this->displayList);
}

void Map::compileDL()
{
	// compile the list
	glNewList(this->displayList, GL_COMPILE);

	// create a char for the frame
	char frame = 0;

	// start drawing the tiles
	for (int y = 0; y < this->header->height; y++)
	{
		for (int x = 0; x < this->header->width; x++)
		{
			frame = this->getTile(x, y);
			if (frame != 0)
			{
				this->drawSprite->setPosition(x * 16, y * 16);
				this->drawSprite->setFrame(frame);
				this->drawSprite->draw();
			}
		}
	}

	// end the list
	glEndList();
}

void Map::setTile(char tile, int x, int y)
{
	this->data->tiles[x + (this->header->width * y)] = tile;
}

char Map::getTile(int x, int y)
{
	return this->data->tiles[x + (this->header->width * y)];
}

MapCrystal *Map::addCrystal(int x, int y)
{
	MapCrystal *crystal = new MapCrystal(x, y);
	this->crystalPile[XY2I(x, y, this->texture->width)] = crystal;
	this->data->crystalCount++;
	return crystal;
}

void Map::removeCrystal(int x, int y)
{
	this->data->crystalCount--;
	this->crystalPile.erase(this->crystalPile.find(XY2I(x, y, this->texture->width)));
}

MapHeader::MapHeader(int w, int h)
{
	strcpy(this->magic, "PSMF");
	this->width = w;
	this->height = h;
}

MapData::MapData(MapHeader *header)
{
	this->tiles = (uint8_t*)malloc(header->width * header->height);
	memset(this->tiles, 1, header->width * header->height);
	this->crystalCount = 0;
	this->crystals = NULL;
}

MapData::~MapData()
{
	free(this->tiles);
	free(this->crystals);
}

MapCrystal::MapCrystal(int x, int y)
{
	this->x = x;
	this->y = y;
}
