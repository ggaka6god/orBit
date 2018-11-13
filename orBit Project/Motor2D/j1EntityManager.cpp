// ----------------------------------------------------
// Controls all logic entities (enemies/player/etc.) --
// ----------------------------------------------------

#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1EntityManager.h"
#include "j1Entity.h"

j1EntityManager::j1EntityManager() : j1Module(), logic_updates_per_second(DEFAULT_LOGIC_PER_SECOND), accumulated_time(0.0f)
{
	name.create("entities");
}

// Destructor
j1EntityManager::~j1EntityManager()
{}


// Called before render is available
bool j1EntityManager::Awake()
{
	LOG("Setting up Entity manager");
	bool ret = true;
	logic_updates_per_second = DEFAULT_LOGIC_PER_SECOND;
	update_ms_cycle = 1.0f / (float)logic_updates_per_second;
	return ret;
}

// Called before the first frame
bool j1EntityManager::Start()
{
	LOG("start j1EntityManager");
	bool ret = true;
	return ret;
}

// Called each loop iteration
bool j1EntityManager::PreUpdate()
{
	do_logic = false;
	return true;
}

bool j1EntityManager::Update(float dt)
{
	accumulated_time += dt;

	if (accumulated_time >= update_ms_cycle)
	{
		do_logic = true;
	}

	UpdateEntity(dt);

	if (do_logic == true)
	{
		//LOG("Did logic step after %f", accumulated_time);
		accumulated_time = accumulated_time - update_ms_cycle;
	}

	return true;
}

void j1EntityManager::UpdateEntity(float dt)
{
	p2List_item <j1Entity*> *entity = entities.start;

	while (entity != NULL)
	{
		entity->data->FixedUpdate(dt);

		if (do_logic == true)
		{
			entity->data->LogicUpdate(accumulated_time);
		}

		entity = entity->next;
	}
}

bool j1EntityManager::PostUpdate()
{
	return true;
}

// Called before quitting
bool j1EntityManager::CleanUp()
{
	LOG("cleanup j1EntityManager");
	// release all entities
	p2List_item<j1Entity*>* entity = entities.start;

	while (entity != NULL)
	{
		RELEASE(entity->data);
		entity = entity->next;
	}

	entities.clear();
	return true;
}

// Create a new empty entity
j1Entity* const j1EntityManager::CreateEntity()
{
	j1Entity* entity = new j1Entity;
	entity->Init(this);
	entities.add(entity);
	return(entity);
}

void j1EntityManager::DestroyEntity(j1Entity* entity)
{
	p2List_item <j1Entity*> *entity_item = entities.At(entities.find(entity));
	
	entities.del(entity_item);
}

void j1EntityManager::OnCollision(Collider * c1, Collider * c2)
{
	p2List_item <j1Entity*> *entity = entities.start;

	while (entity != NULL)
	{
		if (entity->data->entitycoll == c1)
		{
			entity->data->OnCollision(c1, c2);
			break;
		}
	  entity = entity->next;
	}
}
