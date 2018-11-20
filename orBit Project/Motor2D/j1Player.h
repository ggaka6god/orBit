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

	bool Load(pugi::xml_node&);
	bool Save(pugi::xml_node&) const;



	// --- Collisions Handling ---

	void OnCollision(Collider* c1, Collider* c2);

	void Right_Collision(Collider* entitycollider, const Collider* to_check);

	void Left_Collision(Collider* entitycollider, const Collider* to_check);

	void Up_Collision(Collider* entitycollider, const Collider* to_check);

	void Down_Collision(Collider* entitycollider ,const Collider* to_check);

public:

	int parallaxflow = 0;
	int previousflow = 0;

	Playerdata playerinfo;

	// --- NEW APPROACH VARIABLES ---

	fPoint Current_position = { 0,0 };
	fPoint Future_position= { 0,0 };


	MOVEMENT EntityMovement = MOVEMENT::STATIC;

};

#endif // __j1Player_H__
