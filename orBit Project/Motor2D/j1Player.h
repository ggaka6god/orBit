#ifndef __j1Player_H__
#define __j1Player_H__

#include "p2Point.h"
#include "Animation.h"
#include "j1Entity.h"

struct SDL_Texture;
struct Collider;

struct Playerdata {

	Animation* idleRight = nullptr;
	Animation* idleLeft = nullptr;
	Animation* runRight = nullptr;
	Animation* runLeft = nullptr;
	Animation* jumpingRight = nullptr;
	Animation* jumpingLeft = nullptr;
	Animation* fallingRight = nullptr;
	Animation* fallingLeft = nullptr;
	Animation* deathRight = nullptr;
	Animation* deathLeft = nullptr;
	Animation* airRight = nullptr;
	Animation* airLeft = nullptr;

	p2SString folder = nullptr;
	p2SString Texture = nullptr;

	SDL_Rect playerrect = { 0,0,0,0 };

	float           gravity = 0;
};

enum class MOVEMENT 
{
	RIGHTWARDS,
	LEFTWARDS,
	UPWARDS,
	FREEFALL,

	STATIC
};


class j1Player :public j1Entity
{
public:

	j1Player();
	~j1Player();

	bool Start();
	bool Update(float dt);
	bool PostUpdate(float dt);
	bool CleanUp();

	// Called each loop iteration
	void FixedUpdate(float dt);
	
	// Called each logic iteration
	void LogicUpdate(float dt);

	void UpdateEntityMovement(float dt);

	void OnCollision(Collider* c1, Collider* c2);

	bool Load(pugi::xml_node&);
	bool Save(pugi::xml_node&) const;

public:

	int parallaxflow = 0;
	int previousflow = 0;

	Playerdata playerinfo;

	// --- NEW APPROACH VARIABLES ---

	iPoint Current_position = { 0,0 };
	iPoint Future_position= { 0,0 };

	MOVEMENT EntityMovement = MOVEMENT::STATIC;

};

#endif // __j1Player_H__
