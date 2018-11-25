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
	entitycoll = App->coll->AddCollider(entitycollrect, COLLIDER_TYPE::COLLIDER_ENEMY_SLIME, (j1Module*)manager);

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

	//if (!App->scene->player->dead)
	//{

		if ((position.x)*App->win->GetScale() >= -App->render->camera.x && (position.x)*App->win->GetScale() <= -App->render->camera.x + App->render->camera.w)
		{
			//check for player nearby

			if (App->scene->player->Future_position.x > position.x - Slimeinfo.areaofaction &&
				App->scene->player->Future_position.x < position.x + Slimeinfo.areaofaction &&
				App->scene->player->Future_position.y < position.y + Slimeinfo.areaofaction &&
				App->scene->player->Future_position.y > position.y - Slimeinfo.areaofaction)
			{
				if (App->scene->player->Future_position.x > position.x && entitystate != FALLING)
				{
					CurrentAnimation = Slimeinfo.runRight;
					entitystate = RIGHT;
					going_right = true;
				}

				else if (App->scene->player->Future_position.x < position.x && entitystate != FALLING)
				{
					CurrentAnimation = Slimeinfo.runLeft;
					entitystate = LEFT;
					going_right = false;
				}
				else if (App->scene->player->Future_position.x == position.x && entitystate != FALLING)
				{
					CurrentAnimation = Slimeinfo.runRight;
					entitystate = IDLE;
					going_right = false;
				}

				CreatePathfinding({ (int)App->scene->player->Future_position.x, (int)App->scene->player->Future_position.y });

				Pathfind(dt);

			}
			else
			{
				if (Slimeinfo.Velocity != Slimeinfo.auxVel)
				{
					Slimeinfo.Velocity = Slimeinfo.auxVel;
				}

				if (entitystate != FALLING && entitystate == RIGHT)
				{
					position.x += Slimeinfo.Velocity.x*dt;
					entitystate = RIGHT;
					must_fall = false;
				}

				else if (entitystate != FALLING && entitystate == LEFT)
				{
					position.x -= Slimeinfo.Velocity.x*dt;
					entitystate = LEFT;
					must_fall = false;
				}
			}

			if (entitystate == RIGHT)
				CurrentAnimation = Slimeinfo.runRight;
			else if (entitystate == LEFT)
				CurrentAnimation = Slimeinfo.runLeft;

			//If no ground, free fall
			if (must_fall)
			{
				position.y -= gravity*dt;
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
		}
	//}

	//Blitting slime
	App->render->Blit(spritesheet, position.x - Slimeinfo.printingoffset.x, position.y + Slimeinfo.printingoffset.y, &CurrentAnimation->GetCurrentFrame(dt));

	return ret;
}



void j1Slime::OnCollision(Collider * c1, Collider * c2)
{
	bool lateralcollision = true;

	if (c1->rect.y + c1->rect.h >= c2->rect.y && c1->rect.y + c1->rect.h <= c2->rect.y + 3)
	{
		lateralcollision = false;
	}

	if (c2->type == COLLIDER_TYPE::COLLIDER_FLOOR || c2->type == COLLIDER_TYPE::COLLIDER_PLATFORM && dead == false && !lateralcollision)
	{
		if (c1->rect.y + c1->rect.h > c2->rect.y && c1->rect.y + c1->rect.h<c2->rect.y + colliding_offset)
		{
			c1->rect.y = c2->rect.y - c1->rect.h;
		}

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
			c1->rect.x = c2->rect.x - c1->rect.w - 2.0f;
		}
		else
		{
			going_right = true;
			entitystate = RIGHT;
			going_left = false;
			c1->rect.x = c2->rect.x + c2->rect.w + 2.0f;
		}
		slimecolliding = true;
			
		position.x = c1->rect.x;
	}

}

bool j1Slime::ReestablishVariables()
{
	bool ret = true;

	pathfinding_size = 0;

	return ret;
}

bool j1Slime::CreatePathfinding(const iPoint destination)
{
	bool ret = false;

	if (App->scene->firstStage == true)
	{
		if (App->pathfinding->CreatePath(App->map->WorldToMap(position.x, position.y, App->map->data), App->map->WorldToMap(destination.x, destination.y, App->map->data)))
		{
			last_pathfinding = App->pathfinding->GetLastPath();
			pathfinding_size = last_pathfinding->Count();
			pathfinding_index = 1;
			current_path.Clear();

			for (int i = 0; i < pathfinding_size; ++i) {
				current_path.PushBack(*last_pathfinding->At(i));
				ret = true;
			}
		}
	}

	else
	{
		if (App->pathfinding->CreatePath(App->map->WorldToMap(position.x, position.y, App->map->data2), App->map->WorldToMap(destination.x, destination.y, App->map->data2)))
		{
			last_pathfinding = App->pathfinding->GetLastPath();
			pathfinding_size = last_pathfinding->Count();
			pathfinding_index = 1;
			current_path.Clear();

			for (int i = 0; i < pathfinding_size; ++i) {
				current_path.PushBack(*last_pathfinding->At(i));
				ret = true;
			}
		}
	}

	return ret;
}

bool j1Slime::Pathfind(float dt)
{
	bool ret = true;

	if (pathfinding_size > 1) {
		if (App->scene->firstStage == true)
		{
			iPoint next_node = App->map->MapToWorld(current_path[pathfinding_index].x, current_path[pathfinding_index].y, App->map->data);
			UpdateMovement(dt);

			if (App->map->WorldToMap(position.x, position.y, App->map->data) == App->map->WorldToMap(next_node.x, next_node.y, App->map->data)) {
				if (pathfinding_index < pathfinding_size - 1)
					pathfinding_index++;
			}
			if (App->map->WorldToMap(position.x, position.y, App->map->data) == current_path[pathfinding_size - 1])
				ret = false;
		}

		else
		{
			iPoint next_node = App->map->MapToWorld(current_path[pathfinding_index].x, current_path[pathfinding_index].y, App->map->data2);
			UpdateMovement(dt);

			if (App->map->WorldToMap(position.x, position.y, App->map->data2) == App->map->WorldToMap(next_node.x, next_node.y, App->map->data2)) {
				if (pathfinding_index < pathfinding_size - 1)
					pathfinding_index++;
			}
			if (App->map->WorldToMap(position.x, position.y, App->map->data2) == current_path[pathfinding_size - 1])
				ret = false;
		}
	}
	else
		ret = false;

	return ret;
}

void j1Slime::UpdateMovement(float dt)
{
	Slimeinfo.Velocity.x = current_path[pathfinding_index].x - App->map->WorldToMap(position.x, position.y, App->map->data).x;
	Slimeinfo.Velocity.y = current_path[pathfinding_index].y - App->map->WorldToMap(position.x, position.y, App->map->data).y;

	Slimeinfo.Velocity.x = Slimeinfo.Velocity.x*50.0f * dt;
	Slimeinfo.Velocity.y = Slimeinfo.Velocity.y*50.0f * dt;
	position.x += Slimeinfo.Velocity.x;
	position.y += Slimeinfo.Velocity.y;
}



bool j1Slime::Load(pugi::xml_node &config)
{
	bool ret = true;
	if (entityID== Slimeinfo.RefID.x)
	{
		position.x = config.child("Entity4").child("Slimex").attribute("value").as_float();
		position.y = config.child("Entity4").child("Slimey").attribute("value").as_float();
	}
	else if (entityID== Slimeinfo.RefID.y)
	{
		position.x = config.child("Entity5").child("Slimex").attribute("value").as_float();
		position.y = config.child("Entity5").child("Slimey").attribute("value").as_float();
	}
	


	return ret;
}

bool j1Slime::Save(pugi::xml_node &config) const
{
	if (entityID == Slimeinfo.RefID.x)
	{
		config.append_child("Entity4").append_child("Slimex").append_attribute("value") = position.x;
		config.child("Entity4").append_child("Slimey").append_attribute("value") = position.y;
	}
	else if (entityID == Slimeinfo.RefID.y)
	{
		config.append_child("Entity5").append_child("Slimex").append_attribute("value") = position.x;
		config.child("Entity5").append_child("Slimey").append_attribute("value") = position.y;
	}

	return true;
}

bool j1Slime::CleanUp()
{
	bool ret = true;
	
	delete path_info;

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
	App->coll->QueryCollisions(*entitycoll);
}
