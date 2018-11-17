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
#include "j1Pathfinding.h"

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
	entitycoll = App->coll->AddCollider(entitycollrect, COLLIDER_ENEMY_SLIME, (j1Module*)manager);

	CurrentAnimation = Slimeinfo.runRight;
	Slimeinfo.runLeft->speed = Slimeinfo.animationspeed;
	Slimeinfo.runRight->speed = Slimeinfo.animationspeed;

	gravity = Slimeinfo.gravity;

	position.x = NULL;
	position.y = NULL;

	entitystate = FALLING;

	going_right = true;
	going_left = false;
	must_fall = true;
	dead = false;
	
	if (spritesheet == nullptr)
		spritesheet = App->tex->Load(Slimeinfo.Texture.GetString());

	entityID = App->entities->entityID;

	
	
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
		//check for player nearby

		if (App->scene->player->position.x > position.x - Slimeinfo.areaofaction &&
			App->scene->player->position.x < position.x + Slimeinfo.areaofaction &&
			App->scene->player->position.y < position.y + Slimeinfo.areaofaction &&
			App->scene->player->position.y > position.y - Slimeinfo.areaofaction)
		{
			if (App->scene->player->position.x > position.x && entitystate != FALLING)
			{
				CurrentAnimation = Slimeinfo.runRight;
				entitystate = RIGHT;
				going_right = true;

			}
				
			else if (App->scene->player->position.x < position.x && entitystate != FALLING)
			{
				CurrentAnimation = Slimeinfo.runLeft;
				entitystate = LEFT;
				going_right = false;
			}
			else if (App->scene->player->position.x == position.x && entitystate != FALLING)
			{
				CurrentAnimation = Slimeinfo.runRight;
				entitystate = IDLE;
				going_right = false;

			}


			//int pathok= App->pathfinding->CreatePath({ (int)App->scene->player->position.x,(int)App->scene->player->position.y }, { (int)this->position.x, (int)this->position.y });
			//path=App->pathfinding->GetLastPath();

		}
		
				if (entitystate != FALLING && entitystate == RIGHT)
						{
							position.x += Slimeinfo.Velocity.x;
							entitystate = RIGHT;
							must_fall = false;
						}
				else if (entitystate != FALLING && entitystate == LEFT)
						{
							position.x -= Slimeinfo.Velocity.x;
							entitystate = LEFT;
							must_fall = false;
						}


				if (entitystate == RIGHT)
							CurrentAnimation = Slimeinfo.runRight;
				else if (entitystate == LEFT)
							CurrentAnimation = Slimeinfo.runLeft;
		
		

		//If no ground, free fall
		if (must_fall)
		{
			position.y -= gravity;
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

	
			App->render->Blit(spritesheet, position.x - Slimeinfo.printingoffset.x, position.y + Slimeinfo.printingoffset.y, &CurrentAnimation->GetCurrentFrame());
		
		

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
			c1->rect.x -= Slimeinfo.colliding_offset;
		}
		else
		{
			going_right = true;
			entitystate = RIGHT;
			going_left = false;
			c1->rect.x += Slimeinfo.colliding_offset;
			
		}
		slimecolliding = true;
			
		position.x = c1->rect.x;
	}

}



bool j1Slime::Load(pugi::xml_node &config)
{
	bool ret = true;
	if (entityID==4)
	{
		position.x = config.child("Entity4").child("Slimex").attribute("value").as_float();
		position.y = config.child("Entity4").child("Slimey").attribute("value").as_float();
	}
	else if (entityID==5)
	{
		position.x = config.child("Entity5").child("Slimex").attribute("value").as_float();
		position.y = config.child("Entity5").child("Slimey").attribute("value").as_float();
	}
	


	return ret;
}

bool j1Slime::Save(pugi::xml_node &config) const
{
	if (entityID == 4)
	{
		config.append_child("Entity4").append_child("Slimex").append_attribute("value") = position.x;
		config.child("Entity4").append_child("Slimey").append_attribute("value") = position.y;
	}
	else if (entityID == 5)
	{
		config.append_child("Entity5").append_child("Slimex").append_attribute("value") = position.x;
		config.child("Entity5").append_child("Slimey").append_attribute("value") = position.y;
	}

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
