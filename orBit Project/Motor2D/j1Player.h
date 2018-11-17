#ifndef __j1Player_H__
#define __j1Player_H__

#include "j1Module.h"
#include "p2Point.h"
#include "Animation.h"
#include "j1Entity.h"

struct SDL_Texture;
struct Collider;

struct Playerdata {
	
	float jump_force = 0;
	float initialVx = 0;
	float max_speed_y = 0;

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

	fPoint          Velocity = { 0,0 };
	float           gravity = 0;
	float			colliding_offset = 0;
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

	void OnCollision(Collider* c1, Collider* c2);

	bool Load(pugi::xml_node&);
	bool Save(pugi::xml_node&) const;

public:

	bool playercolliding;
	bool colliding_floor;

	bool double_jump;
	bool must_fall;

	bool going_right;
	bool going_left;

	bool wasRight;
	bool dead;

	bool initialmoment;
	bool first_move;
	bool god_mode;

	int parallaxflow = 0;
	int previousflow = 0;

	Playerdata playerinfo;

};

#endif // __j1Player_H__
