#include "Map.h"

#include "Log.h"

#include <stdio.h>
#include <math.h>
#include <string.h>

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
	// allocate memory for the crystal data
	this->data->crystals = new int16_t[this->data->crystalCount];
	// read the crystal data
	fread(this->data->crystals, 1, sizeof(int16_t) * this->data->crystalCount, file);
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

	// save out the header
	fwrite(this->header, 1, sizeof(MapHeader), file);
	// save the tiles
	fwrite(this->data->tiles, 1, this->header->width * this->header->height, file);
	// save the amount of crystals
	fwrite(&this->data->crystalCount, 1, 1, file);
	// save the crystal data
	fwrite(this->data->crystals, 1, sizeof(uint16_t) * this->data->crystalCount, file);
	// close the file handle
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

	// loop through the crystals
	for (int i = 0; i < this->data->crystalCount; i++)
	{
		// get the index
		uint16_t index = this->data->crystals[i];
		// get the X and Y position
		int x = index % this->header->width;
		int y = floor(index / this->header->width);

		// select the crystal stand to draw
		this->drawSprite->setFrame(3, 1);
		// move to the position of the crystal stand
		this->drawSprite->setPosition(x * 16, y * 16);
		// draw the crystal stand
		this->drawSprite->draw();

		// select the crystal
		this->drawSprite->setFrame(3, 0);
		// move to the position of the crystal stand and up some for the crystal
		this->drawSprite->setPosition(x * 16, (y * 16) - 6);
		// draw the crystal
		this->drawSprite->draw();
	}

	// end the list
	glEndList();
}

void Map::setTile(char tile, int x, int y)
{
	// set the tile data for this position
	this->data->tiles[x + (this->header->width * y)] = tile;
}

char Map::getTile(int x, int y)
{
	// get the tile from the current position
	return this->data->tiles[x + (this->header->width * y)];
}

void Map::addCrystal(int x, int y)
{
	// increase the crystal count
	this->data->crystalCount++;
	// create a temporary crystal array to dynamically increase it
	int16_t *crystals = new int16_t[this->data->crystalCount];
	// copy the crystal data over to the new array
	if (this->data->crystalCount > 1)
		memcpy(crystals, this->data->crystals, sizeof(int16_t) * (this->data->crystalCount - 1));
	// add the new crystal to the array
	crystals[this->data->crystalCount - 1] = (x + (this->header->width * y));
	// delete the old crystals data
	delete[] this->data->crystals;
	// set the new crystal data
	this->data->crystals = crystals;
}

void Map::removeCrystal(int x, int y)
{
	// decrease the crystal count
	this->data->crystalCount--;

	// create a temporary crystal array to dynamically decrease it
	int16_t *crystals = new int16_t[this->data->crystalCount];

	// get the index
	int index = (x + (this->header->width * y));

	// loop through the old array and populate the new one
	int lx = 0;
	for (int i = 0; i < this->data->crystalCount + 1; i++)
		if (this->data->crystals[i] != index)
			crystals[lx++] = this->data->crystals[i];

	// if no crystal was found in this location then we don't actually want to delete it
	if (lx == this->data->crystalCount + 1)
	{
		delete[] crystals;
		this->data->crystalCount++;
		return;
	}
	
	// delete the old data
	delete[] this->data->crystals;
	// set the new crystal data
	this->data->crystals = crystals;
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
	memset(this->tiles, 0, header->width * header->height);
	this->crystalCount = 0;
	this->crystals = NULL;
}

MapData::~MapData()
{
	free(this->tiles);
	free(this->crystals);
}
