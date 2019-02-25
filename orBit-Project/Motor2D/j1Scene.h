#ifndef __j1SCENE_H__
#define __j1SCENE_H__

#include "j1Module.h"

struct SDL_Texture;
struct SDL_Rect;
class j1Player;
class j1Slime;
class j1Bat;

class j1Scene : public j1Module
{
public:

	j1Scene();

	// Destructor
	virtual ~j1Scene();

	// Called before render is available
	bool Awake(pugi::xml_node& config);

	// Called before the first frame
	bool Start();

	// Called before all Updates
	bool PreUpdate();

	// Called each loop iteration
	bool Update(float dt);

	// Called before all Updates
	bool PostUpdate(float dt);

	// Called before quitting
	bool CleanUp();

	//call to save stage info
	bool Save(pugi::xml_node&config) const;

	//call to load file
	bool Load(pugi::xml_node&config);

	bool change_scene(const char* map_name);

public:

	p2List<p2SString*> StageList;
	p2SString FirstStage;
	
	bool firstStage = true;
	bool secondStage = false;
	iPoint camera1;
	iPoint camera2;

	bool DestinationStage1 = false;
	bool DestinationStage2 = false;

	//Entities on map
	j1Player*           player = nullptr;
	j1Slime*			slime = nullptr;
	j1Slime*			slime2 = nullptr;
	j1Bat*				bat = nullptr;
	j1Bat*				bat2 = nullptr;

	//variables used on module collision
	int areaofcollision = 0;

	//variables for save and load
	mutable int xSlime = 0;
	mutable int ySlime = 0;

	mutable int xSlime2 = 0;
	mutable int ySlime2 = 0;

	mutable	int xBat = 0;
	mutable int yBat = 0;

	mutable int xBat2 = 0;
	mutable int yBat2 = 0;

private:
	p2SString map_name=nullptr;
	SDL_Rect debug_Tex_rect = { 96,0,16,16 };
};

#endif // __j1SCENE_H__