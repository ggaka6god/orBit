#include "j1Player.h"
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


j1Player::j1Player() : j1Entity("player",entity_type::PLAYER)
{
}

j1Player::~j1Player()
{

}

bool j1Player::Start()
{
	LOG("Loading player");

	playerinfo = manager->GetPlayerData();

	entitycollrect = playerinfo.playerrect;

	entitycoll = App->coll->AddCollider(entitycollrect, COLLIDER_PLAYER, (j1Module*) manager);

	Velocity = playerinfo.Velocity;
	gravity = playerinfo.gravity;
	colliding_offset = playerinfo.colliding_offset;

	position.x = 0;
	position.y = 0;

	parallaxflow = 0;
	previousflow = 0;

	entitystate = FALLING;
	CurrentAnimation = playerinfo.idleRight;
	must_fall = false;
	double_jump = false;
	playercolliding = false;
	initialmoment = true;
	first_move = false;
	god_mode = false;
	going_right = false;
	going_left = false;
	wasRight = true;
	dead = false;
	colliding_floor = true;
	
	if (spritesheet == nullptr)
		spritesheet = App->tex->Load(playerinfo.Texture.GetString());


	return true;
}

bool j1Player::Update(float dt)
{
	//Player Update

		if (App->input->GetKey(SDL_SCANCODE_F10) == KEY_DOWN)
		{
			if (god_mode == false)
			{
				god_mode = true;
				entitystate = JUMPING;
				playercolliding = false;
			}
			else
				god_mode = false;

		}

		if (initialmoment)
		{
			entitystate = FALLING;
		}

		if (wasRight == true)
			CurrentAnimation = playerinfo.idleRight;
		else if (wasRight == false)
			CurrentAnimation = playerinfo.idleLeft;

		//Check if player is Falling or jumping

		if (Velocity.y < 0.0f && entitystate == JUMPING)
		{
			entitystate = FALLING;
		}

		if (playercolliding == false && entitystate == IDLE)
		{
			entitystate = FALLING;
		}

		//Horizontal Movement 


		if (CurrentAnimation != playerinfo.deathRight && CurrentAnimation != playerinfo.deathLeft)
		{
			if (dead == true)
			{
				dead = false;
				playerinfo.deathLeft->Reset();
				playerinfo.deathRight->Reset();
			}


			if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT)
			{
				if (first_move == false)
				{
					first_move = true;
				}

				Velocity.x = playerinfo.initialVx;

				position.x -= (Velocity.x)*dt;

				going_left = true;
				going_right = false;
				CurrentAnimation = playerinfo.runLeft;
				wasRight = false;
			}

			if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT)
			{

				if (first_move == false)
				{
					first_move = true;
				}

				Velocity.x = playerinfo.initialVx;
				position.x += ((Velocity.x)*1.25f)*dt;
				going_right = true;
				going_left = false;
				CurrentAnimation = playerinfo.runRight;
				wasRight = true;
			}

			if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT && App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT)
			{

				Velocity.x = 0.0f;
				going_left = true;
				going_right = true;
				CurrentAnimation = playerinfo.idleRight;
			}

			if (App->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT && god_mode)
			{
				Velocity.x = playerinfo.initialVx;
				position.y -= Velocity.x*dt;
			}


			if (App->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT && god_mode)
			{
				Velocity.x = playerinfo.initialVx;
				position.y += Velocity.x*dt;
			}


			//Vertical Movement

			if (god_mode == false)
			{
				if (!must_fall)
				{
					if (App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN && playercolliding == true && colliding_floor == true)
					{
						colliding_floor = false;
						Velocity.y = playerinfo.jump_force;
						entitystate = JUMPING;
						playercolliding = false;
						App->audio->PlayFx(App->audio->jumpfx, 0);

					}
				}

				if (entitystate == JUMPING)
				{
					if (going_right)
					{
						CurrentAnimation = playerinfo.jumpingRight;
					}
					else
						CurrentAnimation = playerinfo.jumpingLeft;

					must_fall = false;

					if (double_jump == true && App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN && Velocity.y != playerinfo.jump_force)
					{
						Velocity.y = playerinfo.jump_force / 1.5f;
						position.y -= Velocity.y*dt;
						double_jump = false;

					}

					Velocity.y += (gravity*3.0f)*dt;
					position.y -= ((Velocity.y))*dt;

				}

				if (entitystate == FALLING)
				{
					must_fall = false;

					if (going_right)
					{
						CurrentAnimation = playerinfo.fallingRight;
					}
					else if (going_left)
					{
						CurrentAnimation = playerinfo.fallingLeft;
					}
					else
						CurrentAnimation = playerinfo.fallingRight;

					if (double_jump == true && App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN && Velocity.y != playerinfo.jump_force)
					{
						Velocity.y = playerinfo.jump_force / 1.5f;
						position.y -= Velocity.y*dt;
						double_jump = false;
						App->audio->PlayFx(App->audio->doublejumpfx, 0);
					}

					Velocity.y += (gravity*6.0f)*dt;
					position.y -= ((Velocity.y))*dt; //makes no sense, damping?

				}
			}
		}
		//Limit maximum y axis velocity
		if (Velocity.y < -playerinfo.max_speed_y) ////// was <
			Velocity.y = -playerinfo.max_speed_y;

		//If no ground, free fall
		if (must_fall && !god_mode)
		{
			Velocity.y += (gravity*10.0f)*dt;
			position.y -= Velocity.y*dt;
			if (going_right)
				CurrentAnimation = playerinfo.airRight;
			if (going_left)
				CurrentAnimation = playerinfo.airLeft;
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

bool j1Player::PostUpdate(float dt)
{
	bool ret = true;

	//Parallax movement

	previousflow = parallaxflow;

	parallaxflow = -App->render->camera.x/App->win->GetScale() - App->map->offset;

	if (App->scene->firstStage)
	{
		App->map->paralaxRef[0] -= (parallaxflow-previousflow) / 10.0f;
		App->map->paralaxRef[1] -= (parallaxflow-previousflow) / 3.0f;

	}
	else if (App->scene->secondStage)
	{
		App->map->paralaxRef[0] -= (parallaxflow - previousflow) / 10.0f;
		App->map->paralaxRef[1] -= (parallaxflow - previousflow) / 3.0f;
	}

	//Controlling player position

	if (entitycoll->rect.x <= 2)
	{
		entitycoll->rect.x = position.x = 2;
	}


	if (entitycoll->rect.y + entitycoll->rect.h >= App->map->data.height*App->map->data.tile_height)
	{
		entitycoll->rect.y = position.y = App->map->data.height*App->map->data.tile_height - entitycoll->rect.h;
	}

	//--- God_mode anims ---
	if (god_mode)
	{
		if (going_right)
			CurrentAnimation = playerinfo.jumpingRight;
		else if (going_left)
			CurrentAnimation = playerinfo.jumpingLeft;
		else
			CurrentAnimation = playerinfo.jumpingRight;
	}

	LOG("speedo: %f",CurrentAnimation->speed);

	//Blitting player
	if(going_right)
	App->render->Blit(spritesheet, position.x - 3, position.y, &CurrentAnimation->GetCurrentFrame(dt));
	else if (going_left)
	App->render->Blit(spritesheet, position.x - 6, position.y, &CurrentAnimation->GetCurrentFrame(dt));
	else
	App->render->Blit(spritesheet, position.x - 3, position.y, &CurrentAnimation->GetCurrentFrame(dt));

	return ret;
}

void j1Player::OnCollision(Collider * c1, Collider * c2)
{
	if (!god_mode)
	{
		bool lateralcollision = true;

		if (c1->rect.y + c1->rect.h >= c2->rect.y && c1->rect.y + c1->rect.h <= c2->rect.y +1 )
		{
			lateralcollision = false;
		}

		float aux = c1->rect.y; //pos.y

		if (c2->type == COLLIDER_FLOOR && dead == false)
		{
			if ((going_left || going_right) && must_fall)
			{

				if (c1->rect.x + c1->rect.w >= c2->rect.x && c1->rect.x + c1->rect.w <= c2->rect.x + 4)
				{
					Velocity.x = 0.0f;
					c1->rect.x = c2->rect.x - c1->rect.w - colliding_offset;
				}

				if (c1->rect.x >= c2->rect.x + c2->rect.w - 4 && c1->rect.x <= c2->rect.x + c2->rect.w)
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

					double_jump = false;
					must_fall = true;
				}
				else
				{
					must_fall = false;
				}

			}
			else
			{
				if (entitystate != JUMPING && entitystate != FALLING)
				{
					Velocity.y = 0.0f;
					entitystate = IDLE;
					colliding_floor = true;
					double_jump = true;
				}

				if (entitystate != JUMPING)
				{

					if (going_right == true && going_left == true)
					{
						c1->rect.y = aux;
					}
					else
					{
						c1->rect.y = c2->rect.y - c1->rect.h;
					}
				}

				if (going_right)
				{

					//stopping player if lateral collision

					if (lateralcollision)
					{

						if (c1->rect.x + c1->rect.w >= c2->rect.x && c1->rect.x + c1->rect.w <= c2->rect.x + 4)
						{
							Velocity.x = 0.0f;
							if (entitystate != JUMPING)
								c1->rect.y = aux;
							c1->rect.x = c2->rect.x - c1->rect.w - colliding_offset;
						}

						if (entitystate == JUMPING || entitystate == FALLING && double_jump)
						{
							c1->rect.x -= colliding_offset;
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

						if (c1->rect.x >= c2->rect.x + c2->rect.w - 4 && c1->rect.x <= c2->rect.x + c2->rect.w)
						{
							Velocity.x = 0.0f;
							if (entitystate != JUMPING)
								c1->rect.y = aux;
							c1->rect.x = c2->rect.x + c2->rect.w + colliding_offset;
						}

						if (entitystate == JUMPING || entitystate == FALLING && double_jump)
						{
							c1->rect.x += colliding_offset;
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

		else if (c2->type == COLLIDER_SPIKES)
		{
			Velocity.x = 0.0f;

			must_fall = false;
			double_jump = false;

			if (!god_mode)
			{

				if (going_right)
				{
					CurrentAnimation = playerinfo.deathRight;
				}
				else
				{
					CurrentAnimation = playerinfo.deathLeft;
				}

				if (CurrentAnimation->Finished())
				{
					if (!dead)
						dead = true;
					App->LoadGame("save_game.xml");
				}
				App->audio->PlayFx(App->audio->deathfx, 0);
			}
		}

		else if (c2->type == COLLIDER_PLATFORM && dead == false)
		{
		

			if ((going_left || going_right) && must_fall)
			{

				if (c1->rect.x + c1->rect.w >= c2->rect.x && c1->rect.x + c1->rect.w <= c2->rect.x + 4)
				{
					Velocity.x = 0.0f;
					c1->rect.x = c2->rect.x - c1->rect.w - colliding_offset;
				}

				if (c1->rect.x >= c2->rect.x + c2->rect.w - 4 && c1->rect.x <= c2->rect.x + c2->rect.w)
				{
					Velocity.x = 0.0f;
					c1->rect.x = c2->rect.x + c2->rect.w + colliding_offset;
				}

				if ((c1->rect.y + c1->rect.h >= c2->rect.y && c1->rect.y + c1->rect.h <= c2->rect.y + (8)))
				{

					if (entitystate != JUMPING)
					{
						Velocity.y = 0.0f;
						entitystate = IDLE;

					}

					c1->rect.y = c2->rect.y - c1->rect.h;
					double_jump = true;
					must_fall = false;
				}

			}

			else
			{
				if ((c1->rect.y + c1->rect.h >= c2->rect.y && c1->rect.y + c1->rect.h <= c2->rect.y + (8)))
				{

					if (entitystate != JUMPING)
					{
						Velocity.y = 0.0f;
						entitystate = IDLE;
						colliding_floor = true;
					}

					c1->rect.y = c2->rect.y - c1->rect.h;
					double_jump = true;
					must_fall = false;
				}

			}

		}

		else if (c2->type == COLLIDER_ROOF)
		{

			if (c1->rect.y <= c2->rect.y + c2->rect.h && c1->rect.y >= c2->rect.y + c2->rect.h - 4)
			{
				c1->rect.y = c2->rect.y + c2->rect.h + colliding_offset;
				Velocity.y = 0.0f;
				entitystate = FALLING;
				double_jump = false;
				must_fall = true;
			}
			else
			{

				if ((entitystate == JUMPING || entitystate == FALLING) && going_right || going_left)
				{
					double_jump = false;
					must_fall = true;

				}
				if (going_right)
				{
					c1->rect.x = c2->rect.x - c1->rect.w - colliding_offset;

				}
				else
				{
					c1->rect.x = c2->rect.x + c2->rect.w + colliding_offset;
				}
			}
		}

		else if (c2->type == CHECKPOINT)
		{
			if (going_right)
			{
				if (c1->rect.x >= c2->rect.x + c2->rect.w - colliding_offset)
				{
					App->SaveGame("save_game.xml");
				}

			}
			else
			{
				if (c1->rect.x + c1->rect.w <= c2->rect.x + colliding_offset)
				{
					App->SaveGame("save_game.xml");
				}
			}
		}


		if (first_move)
			initialmoment = false;

		if (initialmoment && !first_move)
		{
			CurrentAnimation = playerinfo.idleRight;
		}

		position.x = c1->rect.x;
		position.y = c1->rect.y;

		if (c2->type != CHECKPOINT)
			playercolliding = true;
		else
			playercolliding = false;
	}
}

bool j1Player::Load(pugi::xml_node &config)
{
	bool ret = true;

	position.x = config.child("Player").child("Playerx").attribute("value").as_float();
	position.y = config.child("Player").child("Playery").attribute("value").as_float();

	return ret;
}

bool j1Player::Save(pugi::xml_node &config) const
{
	config.append_child("Player").append_child("Playerx").append_attribute("value")= position.x;
	config.child("Player").append_child("Playery").append_attribute("value")= position.y;

	return true;
}

bool j1Player::CleanUp()
{
	bool ret = true;
	App->tex->UnLoad(spritesheet);

	if(entitycoll != nullptr)
	entitycoll = nullptr;

	return ret;
}

void j1Player::FixedUpdate(float dt)
{
	PostUpdate(dt);
}

void j1Player::LogicUpdate(float dt)
{
	Update(dt);

	// --- Set player pos, prevent surpassing colliders ---
	entitycoll->SetPos(position.x, position.y);

	App->coll->Update(1.0f);

	entitycoll->SetPos(position.x, position.y);
}
