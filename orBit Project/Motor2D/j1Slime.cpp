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

	position.x = 700;
	position.y = 200;

	entitystate = RIGHT;

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
	//slime Update

	
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

	return true;
}

bool j1Slime::PostUpdate(float dt)
{
	bool ret = true;

	//Blitting slime
		App->render->Blit(spritesheet, position.x-8, position.y, &CurrentAnimation->GetCurrentFrame());
	
	return ret;
}

void j1Slime::OnCollision(Collider * c1, Collider * c2)
{
	
	{
		bool lateralcollision = true;

		if (c1->rect.y + c1->rect.h == c2->rect.y)
		{
			lateralcollision = false;
		}

		float aux = c1->rect.y; //pos.y

		if (c2->type == COLLIDER_FLOOR && dead == false)
		{
			if ((going_left || going_right) && must_fall)
			{
				if (c1->rect.x + c1->rect.w >= c2->rect.x && c1->rect.x + c1->rect.w <= c2->rect.x + Slimeinfo.initialVx)
				{
					Velocity.x = 0.0f;
					c1->rect.x = c2->rect.x - c1->rect.w - colliding_offset;
				}

				if (c1->rect.x >= c2->rect.x + c2->rect.w - Slimeinfo.initialVx && c1->rect.x <= c2->rect.x + c2->rect.w)
				{
					Velocity.x = 0.0f;
					c1->rect.x = c2->rect.x + c2->rect.w + colliding_offset;
				}

				if (lateralcollision == true)
				{

					if (going_left)
						c1->rect.x += colliding_offset;
					else
						c1->rect.x -= colliding_offset;

				
					must_fall = true;
				}
				else
				{
					must_fall = false;
				}

			}
			else
			{
				
				if (going_right)
				{

					//stopping player if lateral collision

					if (lateralcollision)
					{

						if (c1->rect.x + c1->rect.w >= c2->rect.x && c1->rect.x + c1->rect.w <= c2->rect.x + Slimeinfo.initialVx)
						{
							Velocity.x = 0.0f;
							if (entitystate != JUMPING)
								c1->rect.y = aux;
							c1->rect.x = c2->rect.x - c1->rect.w - colliding_offset;
						}

						
					}
					else if (!lateralcollision && must_fall == false)
						entitystate = IDLE;

					if ((going_left || going_right) && must_fall)
					{
						c1->rect.x = c2->rect.x + c2->rect.w - colliding_offset;
					}
				}

				//going left
				if (going_left)
				{

					if (lateralcollision)
					{

						if (c1->rect.x >= c2->rect.x + c2->rect.w - Slimeinfo.initialVx && c1->rect.x <= c2->rect.x + c2->rect.w)
						{
							Velocity.x = 0.0f;
							if (entitystate != JUMPING)
								c1->rect.y = aux;
							c1->rect.x = c2->rect.x + c2->rect.w + colliding_offset;
						}

						
					}
					else if (!lateralcollision && must_fall == false)
						entitystate = IDLE;

					if ((going_left || going_right) && must_fall)
					{
						c1->rect.x = c2->rect.x + c2->rect.w + colliding_offset;
					}
				}

				must_fall = false;

			}
		}

		

		else if (c2->type == COLLIDER_PLATFORM && dead == false)
		{
			/*colliding_roof = false;*/

			if ((going_left || going_right) && must_fall)
			{

				if (c1->rect.x + c1->rect.w >= c2->rect.x && c1->rect.x + c1->rect.w <= c2->rect.x + Slimeinfo.initialVx)
				{
					Velocity.x = 0.0f;
					c1->rect.x = c2->rect.x - c1->rect.w - colliding_offset;
				}

				if (c1->rect.x >= c2->rect.x + c2->rect.w - Slimeinfo.initialVx && c1->rect.x <= c2->rect.x + c2->rect.w)
				{
					Velocity.x = 0.0f;
					c1->rect.x = c2->rect.x + c2->rect.w + colliding_offset;
				}

				if ((c1->rect.y + c1->rect.h >= c2->rect.y && c1->rect.y + c1->rect.h <= c2->rect.y + (-gravity * 8)))
				{

					if (entitystate != JUMPING)
					{
						Velocity.y = 0.0f;
						entitystate = IDLE;

					}

					c1->rect.y = c2->rect.y - c1->rect.h;
					must_fall = false;
				}

			}

			else
			{
				if ((c1->rect.y + c1->rect.h >= c2->rect.y && c1->rect.y + c1->rect.h <= c2->rect.y + (-gravity * 8)))
				{

					if (entitystate != JUMPING)
					{
						Velocity.y = 0.0f;
						entitystate = IDLE;
						//colliding_floor = true;
					}

					c1->rect.y = c2->rect.y - c1->rect.h;
	
					must_fall = false;
				}

			}

		}



		position.x = c1->rect.x;
		position.y = c1->rect.y;

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
