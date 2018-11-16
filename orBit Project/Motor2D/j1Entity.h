#ifndef __J1ENTITY_H__
#define __J1ENTITY_H__

#include "p2SString.h"
#include "p2Point.h"
#include "SDL\include\SDL.h"
#include "Animation.h"
#include "PugiXml\src\pugixml.hpp"

class j1EntityManager;
class Collider;

enum entity_state
{
	IDLE = 0,
	RIGHT,
	LEFT,
	JUMPING,
	FALLING,
	FLYING
};

enum class entity_type
{
	PLAYER,
	SLIME
};

class j1Entity
{
public:

	j1Entity(const char* entname,entity_type entitytype) : manager(NULL), entitytype(entitytype)
	{
		name.create(entname);
	}

	void Init(j1EntityManager* manager)
	{
		this->manager = manager;
	}

	virtual bool Start()
	{
		return true;
	}

	virtual bool Load(pugi::xml_node&)
	{
		return true;
	}

	virtual bool Save(pugi::xml_node&) const
	{
		return true;
	}

	// Called each loop iteration
	virtual void FixedUpdate(float dt)
	{}

	// Called each logic iteration
	virtual void LogicUpdate(float dt)
	{}

	// Called before quitting
	virtual bool CleanUp()
	{
		return true;
	}

	virtual void OnCollision(Collider* c1, Collider* c2)
	{}


	
public:

	// --- Basic ---
	p2SString			name;
	fPoint			position;
	fPoint          Velocity;

	// --- Collider data ---
	Collider*     entitycoll = nullptr;
	SDL_Rect entitycollrect;
	float colliding_offset = 0;

	// --- Gravity ---
	float gravity = 0;

	// --- Entity ---
	entity_type  entitytype;
	entity_state entitystate;

	// --- Animation ---
	Animation* CurrentAnimation = nullptr;

	// --- Spritesheet ---
	SDL_Texture* spritesheet = nullptr;

	j1EntityManager*	manager;
};

#endif // __J1ENTITY_H__
