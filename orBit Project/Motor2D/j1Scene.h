#ifndef __j1SCENE_H__
#define __j1SCENE_H__

#include "j1Module.h"

struct SDL_Texture;
class SDL_Rect;
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

	bool afterLoadingStage1 = false;
	bool afterLoadingStage2 = false;


	j1Player*           player = nullptr;
	j1Slime*			slime = nullptr;
	//j1Slime*			slime2 = nullptr;
	j1Bat*				bat = nullptr;
	//j1Bat*				bat2 = nullptr;
private:
	p2SString map_name=nullptr;
	SDL_Rect debug_Tex_rect = { 96,0,16,16 };
};

#endif // __j1SCENE_H__