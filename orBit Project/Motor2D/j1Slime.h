#ifndef __j1SLIME_H__
#define __j1SLIME_H__

#include "j1Module.h"
#include "p2Point.h"
#include "Animation.h"
#include "j1Entity.h"

struct SDL_Texture;
struct Collider;

struct SlimeData {
	

	
	Animation* runRight = nullptr;
	Animation* runLeft = nullptr;

	p2SString folder = nullptr;
	p2SString Texture = nullptr;

	SDL_Rect SlimeRect = { 0,0,0,0 };
	float           gravity = 0;
	float initialVx = 0;
	float max_speed_y = 0;
	fPoint          Velocity = { 0,0 };
	float  colliding_offset = 0;

};

class j1Slime :public j1Entity
{
public:

	j1Slime();
	~j1Slime();

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
	bool must_fall = false;

	SlimeData Slimeinfo;

};

#endif // __j1SLIME_H__
