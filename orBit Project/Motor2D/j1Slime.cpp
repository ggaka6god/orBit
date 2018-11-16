#include "j1Slime.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Textures.h"
#include "j1Collision.h"
#include "j1Render.h"
#include "j1Input.h"
#include "j1Map.h"
#include "j1Scene.h"
#include "j1Window.h"
#include "j1Audio.h"
#include "j1EntityManager.h"
#include "j1Player.h"


j1Slime::j1Slime() : j1Entity("Slime", entity_type::SLIME)
{
}

j1Slime::~j1Slime()
{

}

bool j1Slime::Start()
{
	LOG("Loading Slime");

	Slimeinfo = manager->GetSlimeData();
	entitycollrect = Slimeinfo.SlimeRect;
	colliding_offset = Slimeinfo.colliding_offset;
	CurrentAnimation = Slimeinfo.runRight;
	Slimeinfo.runLeft->speed = 0.05;
	Slimeinfo.runRight->speed = 0.05;
	gravity = Slimeinfo.gravity;
	entitycoll = App->coll->AddCollider(entitycollrect, COLLIDER_ENEMY_SLIME, (j1Module*)manager);

	position.x = 1000;
	position.y = 100;

	entitystate = FALLING;

	going_right = true;
	going_left = false;
	must_fall = true;
	dead = false;
	
	if (spritesheet == nullptr)
		spritesheet = App->tex->Load(Slimeinfo.Texture.GetString());

	
	return true;
}

bool j1Slime::Update(float dt)
{
	must_fall = true;
	slimecolliding = false;
	entitystate = FALLING;

	return true;
}

bool j1Slime::PostUpdate(float dt)
{
	bool ret = true;


	if ((position.x)*App->win->GetScale() >= -App->render->camera.x && (position.x)*App->win->GetScale() <= -App->render->camera.x + App->render->camera.w)
	{
		//slime Update
		if (entitystate != FALLING && entitystate == RIGHT)
		{
			position.x += 0.5;
			entitystate = RIGHT;
			must_fall = false;
		}
		else if (entitystate != FALLING && entitystate == LEFT)
		{
			position.x -= 0.5;
			entitystate = LEFT;
			must_fall = false;
		}


		if (going_right == true)
			CurrentAnimation = Slimeinfo.runRight;
		else if (going_right == false)
			CurrentAnimation = Slimeinfo.runLeft;

		//If no ground, free fall
		if (must_fall)
		{
			position.y -= gravity * 4.0f;
		}

		if (position.x < 0)
		{
			position.x = 0;
			entitycoll->rect.x = 0;
		}
		else if (position.x > App->map->data.width*App->map->data.tile_width)
		{
			position.x = App->map->data.width*App->map->data.tile_width;
		}

		//Check if slime is Falling 


		if (slimecolliding == false)
		{
			entitystate = FALLING;
		}


		//Blitting slime

		if (going_right)
			App->render->Blit(spritesheet, position.x - 8, position.y + 1, &CurrentAnimation->GetCurrentFrame());
		else if (going_left)
			App->render->Blit(spritesheet, position.x - 8, position.y + 1, &CurrentAnimation->GetCurrentFrame());
		else
			App->render->Blit(spritesheet, position.x - 3, position.y, &CurrentAnimation->GetCurrentFrame());
		

		return ret;
	}
}

void j1Slime::OnCollision(Collider * c1, Collider * c2)
{
	bool lateralcollision = true;

	if (c1->rect.y + c1->rect.h == c2->rect.y)
	{
		lateralcollision = false;
	}

	if (c2->type == COLLIDER_FLOOR || c2->type == COLLIDER_PLATFORM && dead == false && !lateralcollision)
	{
		must_fall = false;
		if (going_right)
		{
			going_right = true;
			entitystate = RIGHT;
			going_left = false;
		}
		else
		{
			entitystate = LEFT;
			going_left = true;
			going_right = false;
		}

		slimecolliding = true;
	}

	if (lateralcollision)
	{
		if (going_right)
		{
			entitystate = LEFT;
			going_left = true;
			going_right = false;
			c1->rect.x -= 5;
		}
		else
		{
			going_right = true;
			entitystate = RIGHT;
			going_left = false;
			c1->rect.x += 5;
			
		}
		slimecolliding = true;
			
		position.x = c1->rect.x;
	}

}



bool j1Slime::Load(pugi::xml_node &config)
{
	bool ret = true;

	position.x = config.child("Slime").child("Playerx").attribute("value").as_float();
	position.y = config.child("Slime").child("Playery").attribute("value").as_float();

	return ret;
}

bool j1Slime::Save(pugi::xml_node &config) const
{
	config.append_child("Slime").append_child("Playerx").append_attribute("value") = position.x;
	config.child("Slime").append_child("Playery").append_attribute("value") = position.y;

	return true;
}

bool j1Slime::CleanUp()
{
	bool ret = true;
	App->tex->UnLoad(spritesheet);

	if (entitycoll != nullptr)
		entitycoll = nullptr;

	return ret;
}

void j1Slime::FixedUpdate(float dt)
{
	PostUpdate(dt);
}

void j1Slime::LogicUpdate(float dt)
{
	Update(dt);

	// --- Set slime pos, prevent surpassing colliders ---
	entitycoll->SetPos(position.x, position.y);

	App->coll->Update(1.0f);

	entitycoll->SetPos(position.x, position.y);
}
