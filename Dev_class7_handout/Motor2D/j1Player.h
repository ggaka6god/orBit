#ifndef __j1Player_H__
#define __j1Player_H__

#include "j1Module.h"
#include "p2Point.h"

struct SDL_Texture;
struct Collider;

enum player_state
{
	IDLE = 0,
	RIGHT,
	LEFT,
	JUMPING,
	FALLING
};

class j1Player :public j1Module
{
public:

	j1Player();
	~j1Player();

	bool Start();
	bool Update(float dt);
	bool PostUpdate();

	void OnCollision(Collider* c1, Collider* c2);

	bool Load(pugi::xml_node&);
	bool Save(pugi::xml_node&) const;

public:

	SDL_Texture* graphics = nullptr;
	Collider* playercollider = nullptr;

	fPoint pos;
	fPoint Velocity;

	float gravity;
	float jump_force;

	player_state stateplayer;
	bool playercolliding;


};

#endif // __j1Player_H__
