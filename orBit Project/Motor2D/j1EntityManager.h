// ----------------------------------------------------
// Controls all logic entities (enemies/player/etc.) --
// ----------------------------------------------------

#ifndef __J1ENTITYMANAGER_H__
#define __J1ENTITYMANAGER_H__

#include "p2List.h"
#include "j1Module.h"
#include "j1Entity.h"
#include "j1Player.h"
#include "j1Slime.h"
#include "j1Bat.h"

#define DEFAULT_LOGIC_PER_SECOND 60

class j1Entity;

class j1EntityManager : public j1Module
{
public:

	j1EntityManager();

	// Destructor
	virtual ~j1EntityManager();

	// Called before render is available
	bool Awake(pugi::xml_node&); // spritesheets and animations

	// Called before the first frame
	bool Start(); // textures

	// Called each loop iteration
	bool PreUpdate();
	bool Update(float dt);
	void UpdateEntity(float dt);
	bool PostUpdate(float dt);

	// Called before quitting
	bool CleanUp();

	// Entities management
	j1Entity * const CreateEntity(const char* entname , entity_type entitytype);
	void DestroyEntity(j1Entity* entity);
	void OnCollision(Collider* c1, Collider* c2);
	Animation* LoadAnimation(const char* animationPath, const char* animationName);

	// --- Get Entities data ---
	Playerdata& GetPlayerData() { return playerinfo; }
	SlimeData& GetSlimeData() { return slimeinfo; }
	BatData& GetBatData() { return batinfo; }

	// --- Save & Load ---
	bool Load(pugi::xml_node&);

	bool Save(pugi::xml_node&) const;
	
public:

	p2List <j1Entity*>	entities;
	bool				do_logic;
	int					logic_updates_per_second;
	float				update_ms_cycle;
	float				accumulated_time;

private:
	// --- Player ---
	Playerdata playerinfo;

	SlimeData slimeinfo;

	BatData batinfo;

};

#endif // __J1ENTITYMANAGER_H__
