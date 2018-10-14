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


public:

	SDL_Texture * graphics = nullptr;
	Collider* playercollider = nullptr;

	fPoint pos;
	fPoint Velocity;

	float gravity;
	float jump_force;
	float initialVx;
	float max_speed_y;
	float colliding_offset;

	player_state stateplayer;
	bool playercolliding;
	bool colliding_roof;

	bool double_jump;
	bool must_fall;

	bool going_right;
	bool going_left;

	bool wasRight = true;
	bool dead = false;

	bool initialmoment;
	bool first_move;
	bool god_mode;

	Animation* CurrentAnimation = nullptr;
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

	SDL_Rect playercol;

private:

	p2SString folder;
	p2SString Texture;
};

#endif // __j1Player_H__
