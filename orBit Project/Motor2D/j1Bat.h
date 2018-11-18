#ifndef __j1BAT_H__
#define __j1BAT_H__

#include "j1Entity.h"

struct SDL_Texture;
struct Collider;
struct PathInfo;

struct BatData {



	Animation* flyRight = nullptr;
	Animation* flyLeft = nullptr;

	p2SString folder = nullptr;
	p2SString Texture = nullptr;

	SDL_Rect		BatRect = { 0,0,0,0 };
	fPoint          Velocity = { 0,0 };
	fPoint          auxVel = { 0,0 };
	iPoint			printingoffset = { 0,0 };

	float           gravity = 0;
	float			initialVx = 0;
	float			colliding_offset = 0;
	float			animationspeed = 0;

	int				areaofaction = 0;
	iPoint			RefID = { 0,0 };

};

class j1Bat :public j1Entity
{
public:

	j1Bat();
	~j1Bat();

	bool Start();
	bool Update(float dt);
	bool PostUpdate(float dt);
	bool CleanUp();

	// Called each loop iteration
	void FixedUpdate(float dt);

	// Called each logic iteration
	void LogicUpdate(float dt);


	void OnCollision(Collider* c1, Collider* c2);

	bool Load(pugi::xml_node&);
	bool Save(pugi::xml_node&) const;

	// --- Pathfinding ---

	bool ReestablishVariables();

	bool CreatePathfinding(const iPoint destination);

	bool Pathfind(float dt);

	void UpdateMovement(float dt);

	// ----------------------

public:
	bool dead = false;
	bool going_right = false;
	bool going_left = false;
	bool going_up = false;
	bool going_down = false;


	bool batcolliding = false;


	BatData BatInfo;

	// Pathfinding
	const p2DynArray<iPoint>* last_pathfinding = nullptr;
	p2DynArray<iPoint> current_path;

	uint pathfinding_index = 0;
	uint pathfinding_size = 0;

	bool create_pathfinding = false;
	bool pathfinding_stop = false;
	bool pathfinding_finished = true;
	bool pathfinding = false;
	bool pathfind = false;
	//_pathfinding

	PathInfo* path_info = nullptr;

};

#endif // __j1BAT_H__
