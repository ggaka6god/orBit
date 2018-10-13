#ifndef __j1Player_H__
#define __j1Player_H__

#include "j1Module.h"
#include "p2Point.h"
#include "Animation.h"

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

	bool Awake(pugi::xml_node &config);
	bool Start();
	bool Update(float dt);
	bool PostUpdate();
	bool CleanUp();

	void OnCollision(Collider* c1, Collider* c2);

	bool Load(pugi::xml_node&);
	bool Save(pugi::xml_node&) const;

	Animation* LoadAnimation(const char* animationPath, const char* animationName);

	void GetCurrentAnimation();

public:

	SDL_Texture * graphics = nullptr;
	Collider* playercollider = nullptr;

	fPoint pos;
	fPoint Velocity;

	float gravity;
	float jump_force;

	player_state stateplayer;
	bool playercolliding;

	float max_speed_y;

	bool double_jump;
	bool must_fall;

	bool going_right;
	bool going_left;

	bool dead = false;


	Animation* CurrectAnimation = nullptr;
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

	SDL_Texture* spritesheet = nullptr;

private:

	p2SString folder;
	p2SString Texture;
};

#endif // __j1Player_H__
