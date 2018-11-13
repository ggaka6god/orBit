// ----------------------------------------------------
// Controls all logic entities (enemies/player/etc.) --
// ----------------------------------------------------

#ifndef __J1ENTITYMANAGER_H__
#define __J1ENTITYMANAGER_H__

#include "p2List.h"
#include "j1Module.h"

#define DEFAULT_LOGIC_PER_SECOND 10

class j1Entity;

class j1EntityManager : public j1Module
{
public:

	j1EntityManager();

	// Destructor
	virtual ~j1EntityManager();

	// Called before render is available
	bool Awake(); // spritesheets and animations

	// Called before the first frame
	bool Start(); // textures

	// Called each loop iteration
	bool PreUpdate();
	bool Update(float dt);
	void UpdateEntity(float dt);
	bool PostUpdate();

	// Called before quitting
	bool CleanUp();

	// Entities management
	j1Entity * const CreateEntity();
	void DestroyEntity(j1Entity* entity);
	void OnCollision(Collider* c1, Collider* c2);

public:

	p2List <j1Entity*>	entities;
	bool				do_logic;
	int					logic_updates_per_second;
	float				update_ms_cycle;
	float				accumulated_time;
};

#endif // __J1ENTITYMANAGER_H__
