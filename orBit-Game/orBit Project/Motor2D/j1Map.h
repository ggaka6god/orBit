#ifndef __j1MAP_H__
#define __j1MAP_H__

#include "PugiXml/src/pugixml.hpp"
#include "p2List.h"
#include "p2Point.h"
#include "j1Module.h"

// ----------------------------------------------------


struct Properties
{
	p2List <p2SString*> name;

	p2List <p2SString*> value;

	bool LoadProperties(pugi::xml_node& node);

	int numproperties;

	p2SString GetProperties(const char * request);

	~Properties()
	{
	}

};

//struct with the infornmation of the parallax layers
struct ImageLayer
{
	SDL_Rect GetParalaxRect() const;

	p2SString	name;
	int			width;
	int			height;
	SDL_Texture* texture;

	ImageLayer() {}

	~ImageLayer() 
	{
		
	}

};

struct MapLayer
{
	p2SString	name;
	int			width;
	int			height;
	uint*		data;
	Properties	properties;

	MapLayer() : data(NULL)
	{}

	~MapLayer()
	{
		RELEASE(data);
	}

	inline uint Get(int x, int y) const
	{
		return data[(y*width) + x];
	}
};

// Tileset information ----------------------------------------------------
struct TileSet
{
	SDL_Rect GetTileRect(int id) const;

	p2SString			name;
	int					firstgid;
	int					margin;
	int					spacing;
	int					tile_width;
	int					tile_height;
	SDL_Texture*		texture;
	int					tex_width;
	int					tex_height;
	int					num_tiles_width;
	int					num_tiles_height;
	int					offset_x;
	int					offset_y;
	
};

enum MapTypes
{
	MAPTYPE_UNKNOWN = 0,
	MAPTYPE_ORTHOGONAL,
	MAPTYPE_ISOMETRIC,
	MAPTYPE_STAGGERED
};
// ----------------------------------------------------
struct MapData
{
	int					width;
	int					height;
	int					tile_width;
	int					tile_height;
	iPoint				initpos;
	iPoint				finalpos;
	SDL_Color			background_color;
	MapTypes			type;
	p2List<TileSet*>	tilesets;
	p2List<MapLayer*>	layers;
	p2List<ImageLayer*> paralaxlist;
	Properties          properties;
};

// ----------------------------------------------------

class j1Map : public j1Module
{
public:

	j1Map();

	// Destructor
	virtual ~j1Map();

	// Called before render is available
	bool Awake(pugi::xml_node& conf);

	// Called each loop iteration
	void Draw(MapData& data);

	// Called before quitting
	bool CleanUp();


	// Load new map
	bool Load(const char* path, MapData& Data);

	iPoint MapToWorld(int x, int y, MapData& Data) const;
	iPoint WorldToMap(int x, int y, MapData& Data) const;
	bool CreateWalkabilityMap(int& width, int& height, uchar** buffer) const;

	bool ColliderDrawer(MapData& data);

private:

	bool LoadMap(MapData& data);
	bool LoadTilesetDetails(pugi::xml_node& tileset_node, TileSet* set);
	bool LoadTilesetImage(pugi::xml_node& tileset_node, TileSet* set);
	bool LoadLayer(pugi::xml_node& node, MapLayer* layer);
	bool LoadParallax(pugi::xml_node& node, ImageLayer* image);

	TileSet* GetTilesetFromTileId(int id,MapData& mapdata) const;

public:

	MapData data;
	MapData data2;

	int numberStages = 0;
	float speed[2];
	float paralaxRef[2];
	int					offset;

private:
	p2SString folder;
	pugi::xml_document	map_file;
	bool				map_loaded;
	int					redCollision;
	int					yellowCollision;
	int					magentaCollision;
	int					greenCollison;
	int					checkpoint;
	int					redCollision2;
	int					yellowCollision2;
	int					magentaCollision2;
	int					greenCollison2;
	int					checkpoint2;
	
};

#endif // __j1MAP_H__