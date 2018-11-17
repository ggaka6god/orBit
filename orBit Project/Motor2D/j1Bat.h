#ifndef __j1BAT_H__
#define __j1BAT_H__

#include "j1Module.h"
#include "p2Point.h"
#include "Animation.h"
#include "j1Entity.h"

struct SDL_Texture;
struct Collider;

struct BatData {



	Animation* flyRight = nullptr;
	Animation* flyLeft = nullptr;

	p2SString folder = nullptr;
	p2SString Texture = nullptr;
	p2SString stringID = nullptr;

	SDL_Rect		BatRect = { 0,0,0,0 };
	fPoint          Velocity = { 0,0 };
	iPoint			printingoffset = { 0,0 };

	float           gravity = 0;
	float			initialVx = 0;
	float			colliding_offset = 0;
	float			animationspeed = 0;

	int				areaofaction = 0;


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

public:
	bool dead = false;
	bool going_right = false;
	bool going_left = false;
	bool going_up = false;
	bool going_down = false;


	bool batcolliding = false;


	BatData BatInfo;

	//const p2DynArray<iPoint>* path;

};

#endif // __j1BAT_H__
