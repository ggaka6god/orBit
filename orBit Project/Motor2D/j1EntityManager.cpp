// ----------------------------------------------------
// Controls all logic entities (enemies/player/etc.) --
// ----------------------------------------------------

#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1EntityManager.h"
#include "Brofiler/Brofiler.h"

j1EntityManager::j1EntityManager() : j1Module()
{
	name.create("entities");
}

// Destructor
j1EntityManager::~j1EntityManager()
{}


// Called before render is available
bool j1EntityManager::Awake(pugi::xml_node& config)
{
	LOG("Setting up Entity manager");
	bool ret = true;
	//logic_updates_per_second = DEFAULT_LOGIC_PER_SECOND;
	//update_ms_cycle = 1.0f / (float)logic_updates_per_second;
    update_ms_cycle = 1.0f / (float)App->framerate_cap;

	LOG("Loading Player Parser");

	pugi::xml_node playernode = config.child("player");

	// --- Player Awake ---
	playerinfo.folder.create(playernode.child("folder").child_value());
	playerinfo.Texture.create(playernode.child("texture").child_value());

	playerinfo.idleRight = LoadAnimation(playerinfo.folder.GetString(), "idle right");
	playerinfo.idleLeft = LoadAnimation(playerinfo.folder.GetString(), "idle left");
	playerinfo.runRight = LoadAnimation(playerinfo.folder.GetString(), "run right");
	playerinfo.runLeft = LoadAnimation(playerinfo.folder.GetString(), "run left");
	playerinfo.jumpingRight = LoadAnimation(playerinfo.folder.GetString(), "jump right");
	playerinfo.jumpingLeft = LoadAnimation(playerinfo.folder.GetString(), "jump left");
	playerinfo.fallingRight = LoadAnimation(playerinfo.folder.GetString(), "air right");
	playerinfo.fallingLeft = LoadAnimation(playerinfo.folder.GetString(), "air left");
	playerinfo.deathRight = LoadAnimation(playerinfo.folder.GetString(), "dead right");
	playerinfo.deathLeft = LoadAnimation(playerinfo.folder.GetString(), "dead left");
	playerinfo.airRight = LoadAnimation(playerinfo.folder.GetString(), "air right");
	playerinfo.airLeft = LoadAnimation(playerinfo.folder.GetString(), "air left");
	int x = playernode.child("collider").attribute("x").as_int();
	int y = playernode.child("collider").attribute("y").as_int();
	int w = playernode.child("collider").attribute("width").as_int();
	int h = playernode.child("collider").attribute("height").as_int();
	playerinfo.playerrect = { x,y,w,h };

	// --- Player main variables ---

	playerinfo.Velocity.x = playernode.child("Velocity").attribute("x").as_float();
	playerinfo.Velocity.y = playernode.child("Velocity").attribute("y").as_float();
	playerinfo.max_speed_y = playernode.child("Velocity").attribute("max_speed_y").as_float();
	playerinfo.initialVx = playernode.child("Velocity").attribute("initalVx").as_float();
	playerinfo.gravity = playernode.child("gravity").attribute("value").as_float();
	playerinfo.jump_force = playernode.child("Velocity").attribute("jump_force").as_float();
	playerinfo.colliding_offset = playernode.child("colliding_offset").attribute("value").as_float();

	playerinfo.idleRight->speed = 10.0f;
	playerinfo.idleLeft->speed = 10.0f;
	playerinfo.runRight->speed = 10.0f;
	playerinfo.runLeft->speed =  10.0f;
	playerinfo.jumpingRight->speed = 10.0f;
	playerinfo.jumpingLeft->speed = 10.0f;
	playerinfo.fallingRight->speed = 10.0f;
	playerinfo.fallingLeft->speed = 10.0f;
	playerinfo.deathRight->speed = 10.0f;
	playerinfo.deathLeft->speed = 10.0f;
	playerinfo.airRight->speed = 10.0f;
	playerinfo.airLeft->speed = 10.0f;

	playerinfo.deathRight->loop = false;
	playerinfo.deathLeft->loop = false;

	//--Slime data load ------------

	pugi::xml_node slimenode = config.child("slime");

	slimeinfo.folder.create(slimenode.child("folder").child_value());
	slimeinfo.Texture.create(slimenode.child("texture").child_value());

	x = slimenode.child("collider").attribute("x").as_int();
	y = slimenode.child("collider").attribute("y").as_int();
	w = slimenode.child("collider").attribute("width").as_int();
	h = slimenode.child("collider").attribute("height").as_int();
	slimeinfo.SlimeRect = { x,y,w,h };


	slimeinfo.runRight= LoadAnimation(slimeinfo.folder.GetString(), "slime right");
	slimeinfo.runLeft = LoadAnimation(slimeinfo.folder.GetString(), "slime left");

	slimeinfo.gravity = slimenode.child("gravity").attribute("value").as_float();
	slimeinfo.Velocity.x = slimeinfo.auxVel.x = slimenode.child("Velocity").attribute("x").as_float();
	slimeinfo.Velocity.y = slimeinfo.auxVel.y = slimenode.child("Velocity").attribute("y").as_float();
	slimeinfo.initialVx = slimenode.child("Velocity").attribute("initalVx").as_float();
	slimeinfo.colliding_offset = slimenode.child("colliding_offset").attribute("value").as_float();
	slimeinfo.areaofaction = slimenode.child("areaofaction").attribute("value").as_int();
	slimeinfo.animationspeed = slimenode.child("animationspeed").attribute("value").as_float();
	slimeinfo.printingoffset.x = slimenode.child("printingoffset").attribute("x").as_int();
	slimeinfo.printingoffset.y = slimenode.child("printingoffset").attribute("y").as_int();
	slimeinfo.RefID.x = slimenode.child("entityID").attribute("value1").as_int();
	slimeinfo.RefID.y = slimenode.child("entityID").attribute("value2").as_int();

	//--- Bat data load --------------------

	pugi::xml_node batnode = config.child("bat");

	batinfo.folder.create(batnode.child("folder").child_value());
	batinfo.Texture.create(batnode.child("texture").child_value());

	x = batnode.child("collider").attribute("x").as_int();
	y = batnode.child("collider").attribute("y").as_int();
	w = batnode.child("collider").attribute("width").as_int();
	h = batnode.child("collider").attribute("height").as_int();
	batinfo.BatRect = { x,y,w,h };


	batinfo.flyRight = LoadAnimation(batinfo.folder.GetString(), "bat right");
	batinfo.flyLeft = LoadAnimation(batinfo.folder.GetString(), "bat left");

	batinfo.gravity = batnode.child("gravity").attribute("value").as_float(); //
	batinfo.Velocity.x = batinfo.auxVel.x = batnode.child("Velocity").attribute("x").as_float();
	batinfo.Velocity.y = batinfo.auxVel.y = batnode.child("Velocity").attribute("y").as_float();
	batinfo.initialVx = batnode.child("Velocity").attribute("initalVx").as_float();
	batinfo.colliding_offset = batnode.child("colliding_offset").attribute("value").as_float();
	batinfo.areaofaction = batnode.child("areaofaction").attribute("value").as_int();
	batinfo.animationspeed = batnode.child("animationspeed").attribute("value").as_float();
	batinfo.printingoffset.x = batnode.child("printingoffset").attribute("x").as_int();
	batinfo.printingoffset.y = batnode.child("printingoffset").attribute("y").as_int();
	batinfo.RefID.x = batnode.child("entityID").attribute("value1").as_int();
	batinfo.RefID.y = batnode.child("entityID").attribute("value2").as_int();

	// ---------------------

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
	BROFILER_CATEGORY("EntityManager_Pre_Update", Profiler::Color::Chartreuse);

	//do_logic = false;
	return true;
}

bool j1EntityManager::Update(float dt)
{
	BROFILER_CATEGORY("EntityManager_Update", Profiler::Color::Chocolate);

	//accumulated_time += dt;

	//if (accumulated_time >= update_ms_cycle)
	//{
	//	do_logic = true;
	//}

	if(dt<update_ms_cycle*1.25f)
	UpdateEntity(dt);

	//if (do_logic == true)
	//{
	//	LOG("Did logic step after %f", accumulated_time);
	//	accumulated_time = 0.0f;
	//	do_logic = false;
	//}

	return true;
}

void j1EntityManager::UpdateEntity(float dt)
{
	p2List_item <j1Entity*> *entity = entities.start;

	while (entity != NULL)
	{

		/*if (do_logic == true)
		{*/
			entity->data->LogicUpdate(dt);
		//}

		entity = entity->next;
	}
}

bool j1EntityManager::PostUpdate(float dt)
{
	BROFILER_CATEGORY("EntityManager_Post_Update", Profiler::Color::Coral);

	p2List_item <j1Entity*> *entity = entities.start;

	while (entity != NULL)
	{
	    entity->data->FixedUpdate(dt);

	    entity = entity->next;
    }

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
		entity->data->CleanUp();
		RELEASE(entity->data);
		entity = entity->next;
	}

	entities.clear();
	return true;
}

bool j1EntityManager::Load(pugi::xml_node &file)
{
	p2List_item <j1Entity*> *entity = entities.start;

	while (entity != NULL)
	{
		entity->data->Load(file);

		entity = entity->next;
	}


	return true;
}

bool j1EntityManager::Save(pugi::xml_node &file) const
{
	p2List_item <j1Entity*> *entity = entities.start;

	while (entity != NULL)
	{
		entity->data->Save(file);

		entity = entity->next;
	}

	return true;
}

// Create a new empty entity
j1Entity* const j1EntityManager::CreateEntity(const char* entname, entity_type entitytype)
{
	j1Entity* entity = nullptr;

	switch (entitytype)
	{
	case entity_type::SLIME:
		entity = new j1Slime();
		break;
	case entity_type::BAT:
		entity = new j1Bat();
		break;
	case entity_type::PLAYER:
		entity = new j1Player();
		break;
	
	}
	entityID++;
	entity->Init(this);
	entity->Start();
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

Animation* j1EntityManager::LoadAnimation(const char* animationPath, const char* animationName) {

	Animation* animation = new Animation();

	bool anim = false;

	pugi::xml_document animationDocument;
	pugi::xml_parse_result result = animationDocument.load_file(animationPath);


	if (result == NULL)
	{
		LOG("Issue loading animation");
	}

	pugi::xml_node objgroup;
	for (objgroup = animationDocument.child("map").child("objectgroup"); objgroup; objgroup = objgroup.next_sibling("objectgroup"))
	{
		p2SString name = objgroup.attribute("name").as_string();
		if (name == animationName)
		{
			anim = true;
			int x, y, h, w;

			for (pugi::xml_node sprite = objgroup.child("object"); sprite; sprite = sprite.next_sibling("object"))
			{
				x = sprite.attribute("x").as_int();
				y = sprite.attribute("y").as_int();
				w = sprite.attribute("width").as_int();
				h = sprite.attribute("height").as_int();

				animation->PushBack({ x, y, w, h });
			}

		}
	}
	if (anim = true)
		return animation;
	else
		return nullptr;

}


