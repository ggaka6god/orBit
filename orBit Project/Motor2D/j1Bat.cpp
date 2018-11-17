#include "j1Bat.h"
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

j1Bat::j1Bat() : j1Entity("Bat", entity_type::BAT)
{
}

j1Bat::~j1Bat()
{

}

bool j1Bat::Start()
{
	LOG("Loading Bat");

	BatInfo = manager->GetBatData();

	entitycollrect = BatInfo.BatRect;
	colliding_offset = BatInfo.colliding_offset;
	entitycoll = App->coll->AddCollider(entitycollrect, COLLIDER_ENEMY_BAT, (j1Module*)manager);

	CurrentAnimation = BatInfo.flyRight;
	BatInfo.flyLeft->speed = BatInfo.animationspeed;
	BatInfo.flyRight->speed = BatInfo.animationspeed;

	gravity = BatInfo.gravity;

	position.x = NULL;
	position.y = NULL;

	entitystate = FLYING;

	going_right = true;
	going_left = false;
	going_down = false;
	going_up = false;
	
	dead = false;

	if (spritesheet == nullptr)
		spritesheet = App->tex->Load(BatInfo.Texture.GetString());

	entityID = App->entities->entityID;


	return true;
}

bool j1Bat::Update(float dt)
{
	going_down = false;
	going_up = false;
	
	batcolliding = false;
	entitystate = FLYING;

	return true;
}

bool j1Bat::PostUpdate(float dt)
{
	bool ret = true;


	if ((position.x)*App->win->GetScale() >= -App->render->camera.x && (position.x)*App->win->GetScale() <= -App->render->camera.x + App->render->camera.w)
	{
		//check for player nearby

		if (App->scene->player->position.x > position.x - BatInfo.areaofaction &&
			App->scene->player->position.x < position.x + BatInfo.areaofaction &&
			App->scene->player->position.y < position.y + BatInfo.areaofaction &&
			App->scene->player->position.y > position.y - BatInfo.areaofaction)
		{
			if (App->scene->player->position.x > position.x )
			{
				CurrentAnimation = BatInfo.flyRight;
				
				going_right = true;

			}

			else if (App->scene->player->position.x < position.x )
			{
				CurrentAnimation = BatInfo.flyLeft;
				
				going_right = false;
			}

			if (App->scene->player->position.y > position.y)
			{
				going_down = false;
				going_up = true;
			}

			else if (App->scene->player->position.y < position.y)
			{
				
				going_down = true;
				going_up = false;
			}

			else if (App->scene->player->position.y == position.y  && App->scene->player->position.x == position.x)

			{
				going_down = false;
				going_up = false;
				going_right = false;
				going_left = false;
				CurrentAnimation = BatInfo.flyRight;

			}

		

		//	//int pathok= App->pathfinding->CreatePath({ (int)App->scene->player->position.x,(int)App->scene->player->position.y }, { (int)this->position.x, (int)this->position.y });
		//	//path=App->pathfinding->GetLastPath();

		}

		//Debug Purpose (moving bat around)
		/*if (App->input->GetKey(SDL_SCANCODE_J) == KEY_REPEAT)
		{
			position.x -= BatInfo.Velocity.x*2;
			going_right=true;
		}
		if (App->input->GetKey(SDL_SCANCODE_L) == KEY_REPEAT)
		{
			position.x += BatInfo.Velocity.x*2;
			going_right = false;
		}
		if (App->input->GetKey(SDL_SCANCODE_I) == KEY_REPEAT)
		{
			position.y -= BatInfo.Velocity.y*2;
			going_up = true;
		}
		if (App->input->GetKey(SDL_SCANCODE_K) == KEY_REPEAT)
		{
			position.y += BatInfo.Velocity.y*2;
			going_down = true;
		}*/

		if (going_right)
		{
			position.x += BatInfo.Velocity.x;
		}
		else if ( !going_right)
		{
			position.x -= BatInfo.Velocity.x;
			
		}

		if (going_up)
		{
			position.y += BatInfo.Velocity.y;

		}
		else if (going_down)
		{
			position.y -= BatInfo.Velocity.y;

		}


		if (going_right)
			CurrentAnimation = BatInfo.flyRight;
		else if (!going_right)
			CurrentAnimation = BatInfo.flyLeft;



		
		//check for limits
		if (position.x < 0)
		{
			position.x = 0;
			entitycoll->rect.x = 0;
		}
		else if (position.x > App->map->data.width*App->map->data.tile_width)
		{
			position.x = App->map->data.width*App->map->data.tile_width;
		}

		

		//Blitting bat

		App->render->Blit(spritesheet, position.x - BatInfo.printingoffset.x, position.y - BatInfo.printingoffset.y, &CurrentAnimation->GetCurrentFrame());

		return ret;
	}
}

void j1Bat::OnCollision(Collider * c1, Collider * c2)
{
	bool lateralcollision = true;

	if (c1->rect.y + c1->rect.h == c2->rect.y || c1->rect.y == c2->rect.y+ c2->rect.h)
	{
		lateralcollision = false;
	}



	if (c2->type == COLLIDER_FLOOR || c2->type == COLLIDER_PLATFORM || c2->type == COLLIDER_ROOF || c2->type == COLLIDER_SPIKES && dead == false && !lateralcollision)
	{
		if (/*going_up &&*/ c2->rect.y + c2->rect.h == c1->rect.y)
		{
			c1->rect.y += BatInfo.colliding_offset;
			going_up = false;
		}
		else if (/*going_down &&*/ c1->rect.y + c1->rect.h == c2->rect.y)
		{
			
			c1->rect.y -= BatInfo.colliding_offset;
			going_down = false;
		}

		batcolliding = true;
	}

	if (lateralcollision)
	{
		if (going_right)
		{
			going_right = false;
			c1->rect.x -= BatInfo.colliding_offset;
		}
		else
		{
			going_right = true;
			c1->rect.x += BatInfo.colliding_offset;

		}
		batcolliding = true;

		
	}


	position.x = c1->rect.x;
	position.y = c1->rect.y;
}



bool j1Bat::Load(pugi::xml_node &config)
{
	bool ret = true;
	if (entityID == BatInfo.RefID.x)
	{
		position.x = config.child("Entity2").child("Batx").attribute("value").as_float();
		position.y = config.child("Entity2").child("Baty").attribute("value").as_float();
	}
	else if (entityID == BatInfo.RefID.y)
	{
		position.x = config.child("Entity3").child("Batx").attribute("value").as_float();
		position.y = config.child("Entity3").child("Baty").attribute("value").as_float();
	}

	return ret;
}

bool j1Bat::Save(pugi::xml_node &config) const
{
	if (entityID == BatInfo.RefID.x)
	{
		config.append_child("Entity2").append_child("Batx").append_attribute("value") = position.x;
		config.child("Entity2").append_child("Baty").append_attribute("value") = position.y;
	}
	else if (entityID == BatInfo.RefID.y)
	{
		config.append_child("Entity3").append_child("Batx").append_attribute("value") = position.x;
		config.child("Entity3").append_child("Baty").append_attribute("value") = position.y;
	}
	return true;
}

bool j1Bat::CleanUp()
{
	bool ret = true;
	App->tex->UnLoad(spritesheet);

	if (entitycoll != nullptr)
		entitycoll = nullptr;

	return ret;
}

void j1Bat::FixedUpdate(float dt)
{
	PostUpdate(dt);
}

void j1Bat::LogicUpdate(float dt)
{
	Update(dt);

	// --- Set batpos, prevent surpassing colliders ---
	entitycoll->SetPos(position.x, position.y);

	App->coll->Update(1.0f);

	entitycoll->SetPos(position.x, position.y);
}
