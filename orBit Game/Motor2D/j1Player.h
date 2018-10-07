#ifndef __j1Player_H__
#define __j1Player_H__

#include "j1Module.h"
#include "p2Point.h"

struct SDL_Texture;
struct Collider;

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
	iPoint playerpos;

	bool movingleft = false;
	bool movingright = false;
	bool jumping = false;

	int jumptime = 3000;

	int currentime = 0;
	int lastTime = 0;
	bool firstime = true;



	fPoint gravity = (0.0f, -10f);
	iPoint pos;

};

#endif // __j1Player_H__