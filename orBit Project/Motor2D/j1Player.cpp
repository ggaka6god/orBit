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

bool j1Player::Awake(pugi::xml_node& config) {

	LOG("Loading Player Parser");
	bool ret = true;

	folder.create(config.child("folder").child_value());
	Texture.create(config.child("texture").child_value());

	idleRight = LoadAnimation(folder.GetString(), "idle right");
	idleLeft = LoadAnimation(folder.GetString(), "idle left");
	runRight = LoadAnimation(folder.GetString(), "run right");
	runLeft = LoadAnimation(folder.GetString(), "run left");
	jumpingRight = LoadAnimation(folder.GetString(), "jump right");
	jumpingLeft = LoadAnimation(folder.GetString(), "jump left");
	fallingRight = LoadAnimation(folder.GetString(), "air right");
	fallingLeft = LoadAnimation(folder.GetString(), "air left");
	deathRight = LoadAnimation(folder.GetString(), "dead right");
	deathLeft = LoadAnimation(folder.GetString(), "dead left");
	airRight = LoadAnimation(folder.GetString(), "air right");
	airLeft = LoadAnimation(folder.GetString(), "air left");
	int x = config.child("collider").attribute("x").as_int();
	int y = config.child("collider").attribute("y").as_int();
	int w = config.child("collider").attribute("width").as_int();
	int h = config.child("collider").attribute("height").as_int();
	playercol = { x,y,w,h-1 };
	
	parallaxflow = 0;
	previousflow = 0;

	pos.x = 0;
	pos.y = 0;

	//Main player variables

	Velocity.x = config.child("Velocity").attribute("x").as_float();
	Velocity.y = config.child("Velocity").attribute("y").as_float();
	max_speed_y = config.child("Velocity").attribute("max_speed_y").as_float();
	initialVx = config.child("Velocity").attribute("initalVx").as_float();
	gravity = config.child("gravity").attribute("value").as_float();
	jump_force = config.child("Velocity").attribute("jump_force").as_float();
	colliding_offset = config.child("colliding_offset").attribute("value").as_float();

	runRight->speed = 0.15f;
	runLeft->speed = 0.15f;

	CurrentAnimation = idleRight;

	deathRight->loop = false;
	deathLeft->loop = false;

	return ret;
}

j1Player::j1Player()
{
	name.create("player");

}

j1Player::~j1Player()
{

}

bool j1Player::Start()
{

	LOG("Loading player");

	playercollider = App->coll->AddCollider(playercol, COLLIDER_PLAYER, this);

	stateplayer = IDLE;
	must_fall = false;
	double_jump = false;
	playercolliding = false;
	initialmoment = true;
	first_move = false;
	god_mode = false;
	colliding_roof = false;
	going_right = false;
	going_left = false;
	wasRight = true;
	dead = false;

	if (spritesheet == nullptr)
		spritesheet = App->tex->Load(Texture.GetString());


	return true;
}

bool j1Player::Update(float dt)
{
	//Player Update

	if (App->input->GetKey(SDL_SCANCODE_F10) == KEY_DOWN)
	{
		if (god_mode == false)
			god_mode = true;
		else
			god_mode = false;

		
	}

	if (initialmoment)
	{
		stateplayer = FALLING;
	}

	if (wasRight==true)
		CurrentAnimation = idleRight;
	else if (wasRight==false)
		CurrentAnimation = idleLeft;

	//Check if player is Falling or jumping

	if (Velocity.y < 0 && stateplayer == JUMPING)
	{
		stateplayer = FALLING;
	}

	if (playercolliding == false && stateplayer == IDLE)
	{
		stateplayer = FALLING;
	}

	//Horizontal Movement 

	
	if (CurrentAnimation != deathRight && CurrentAnimation != deathLeft)
	{
		if (dead == true)
		{
			dead = false;
			deathLeft->Reset();
			deathRight->Reset();
		}

		if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT)
		{
			if (first_move == false)
			{
				first_move = true;
			}

			Velocity.x = initialVx;  
			pos.x = pos.x - Velocity.x;
			going_left = true;
			going_right = false;
			CurrentAnimation = runLeft;
			wasRight = false;
		}

		if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT)
		{

			if (first_move == false)
			{
				first_move = true;
			}
			
			Velocity.x = initialVx;     
			pos.x = pos.x + Velocity.x;
			going_right = true;
			going_left = false;
			CurrentAnimation = runRight;
			wasRight = true;
		}

		if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT && App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT)
		{

			Velocity.x = 0.0f;
			going_left = true;
			going_right = true;
			CurrentAnimation = idleRight;
		}

		//Vertical Movement


		if (!must_fall)
		{
			if (App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN && playercolliding == true)
			{

				Velocity.y = jump_force;
				stateplayer = JUMPING;
				playercolliding = false;
				App->audio->PlayFx(App->audio->jumpfx, 0);

			}
		}

		if (stateplayer == JUMPING)
		{
			if (going_right)
			{
				CurrentAnimation = jumpingRight;
			}
			else
				CurrentAnimation = jumpingLeft;

			must_fall = false;
			if (double_jump == true && App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN && Velocity.y != jump_force)
			{
				Velocity.y = jump_force / 1.5f;
				pos.y -= Velocity.y;
				double_jump = false;
				
			}

			Velocity.y += gravity / 3;
			pos.y -= Velocity.y;

		}

		if (stateplayer == FALLING && !colliding_roof)
		{
			must_fall = false;

			if (going_right)
			{
				CurrentAnimation = fallingRight;
			}
			else if(going_left)
			{
				CurrentAnimation = fallingLeft;
			}
			else
				CurrentAnimation = fallingRight;

			if (double_jump == true && App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN && Velocity.y != jump_force)
			{
				Velocity.y = jump_force / 1.5f;
				pos.y -= Velocity.y;
				double_jump = false;
				App->audio->PlayFx(App->audio->doublejumpfx, 0);
			}

			Velocity.y += gravity / 3;
			pos.y -= Velocity.y;

		}
	}

	//Limit maximum y axis velocity
	if (Velocity.y < -max_speed_y)
		Velocity.y = -max_speed_y;


	//Set player pos, prevent surpassing colliders 
	playercollider->SetPos(pos.x, pos.y);

	App->coll->Update(1.0f);

	playercollider->SetPos(pos.x, pos.y);

	//If no ground, free fall
	if (must_fall)
	{
		pos.y -= gravity*4.0f;
		if(going_right)
		CurrentAnimation = airRight;
		if(going_left)
		CurrentAnimation = airLeft;
	}

	if (pos.x < 0)
	{
		pos.x = 0;
		playercollider->rect.x = 0;
	}
	else if (pos.x > App->map->data.width*App->map->data.tile_width)
	{
		pos.x = App->map->data.width*App->map->data.tile_width;
	}
	

	return true;
}

bool j1Player::PostUpdate()
{
	bool ret = true;

	//Parallax movement

	previousflow = parallaxflow;

	parallaxflow = pos.x - App->map->offset;

	if (App->scene->firstStage)
	{
		App->map->paralaxRef[0] -= (parallaxflow-previousflow) / 2;
		App->map->paralaxRef[1] -= (parallaxflow-previousflow) / 1.25;

	}
	else if (App->scene->secondStage)
	{
		App->map->paralaxRef[0] -= (parallaxflow - previousflow) / 2;
		App->map->paralaxRef[1] -= (parallaxflow - previousflow) / 1.25f;
	}

	//Controlling player position

	if (playercollider->rect.x <= initialVx)  
	{
		playercollider->rect.x = pos.x = initialVx;  
	}

	if (playercollider->rect.y + playercollider->rect.h >= App->map->data.height*App->map->data.tile_height)
	{
		playercollider->rect.y = App->map->data.height*App->map->data.tile_height - playercollider->rect.h;
	}

	//Blitting player
	if(going_right)
	App->render->Blit(spritesheet, pos.x - 3, pos.y, &CurrentAnimation->GetCurrentFrame());
	else if (going_left)
	App->render->Blit(spritesheet, pos.x - 6, pos.y, &CurrentAnimation->GetCurrentFrame());
	else
	App->render->Blit(spritesheet, pos.x - 3, pos.y, &CurrentAnimation->GetCurrentFrame());



	return ret;
}

void j1Player::OnCollision(Collider * c1, Collider * c2)
{
	bool lateralcollision = true;

	if (c1->rect.y + c1->rect.h == c2->rect.y)
	{
		lateralcollision = false;
	}


	float aux = c1->rect.y; //pos.y

	if (c2->type == COLLIDER_FLOOR && dead == false)
	{
		colliding_roof = false;
	
			if ((going_left || going_right) && must_fall)
			{

				if (c1->rect.x + c1->rect.w >= c2->rect.x && c1->rect.x + c1->rect.w <= c2->rect.x + initialVx) 
				{
					Velocity.x = 0.0f;
					c1->rect.x = c2->rect.x - c1->rect.w - 0.1f;
				}

				if (c1->rect.x >= c2->rect.x + c2->rect.w - initialVx && c1->rect.x <= c2->rect.x + c2->rect.w)
				{
					Velocity.x = 0.0f;
					c1->rect.x = c2->rect.x + c2->rect.w + 0.1f;
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
					double_jump = true;
					must_fall = false;
				}

			}
			else
			{
				if (stateplayer != JUMPING && stateplayer != FALLING)
				{
					Velocity.y = 0.0f;
					stateplayer = IDLE;
				}

				if (stateplayer != JUMPING)
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

						if (c1->rect.x + c1->rect.w >= c2->rect.x && c1->rect.x + c1->rect.w <= c2->rect.x + initialVx)
						{
							Velocity.x = 0.0f;
							if (stateplayer != JUMPING)
								c1->rect.y = aux;
							c1->rect.x = c2->rect.x - c1->rect.w;
						}

						if (stateplayer == JUMPING || stateplayer == FALLING && double_jump)
						{
							c1->rect.x -= colliding_offset;
						}
					}
					else if (!lateralcollision && must_fall == false)
						stateplayer = IDLE;

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

						if (c1->rect.x >= c2->rect.x + c2->rect.w - initialVx && c1->rect.x <= c2->rect.x + c2->rect.w)
						{
							Velocity.x = 0.0f;
							if (stateplayer != JUMPING)
								c1->rect.y = aux;
							c1->rect.x = c2->rect.x + c2->rect.w;
						}

						if (stateplayer == JUMPING || stateplayer == FALLING && double_jump)
						{
							c1->rect.x += colliding_offset;
						}
					}
					else if (!lateralcollision && must_fall == false)
						stateplayer = IDLE;

					if ((going_left || going_right) && must_fall)
					{
						c1->rect.x = c2->rect.x + c2->rect.w + colliding_offset;
					}
				}

				double_jump = true;
				must_fall = false;

			}
		}

		else if (c2->type == COLLIDER_SPIKES)
		{
			Velocity.x = 0.0f;

			colliding_roof = false;
			must_fall = false;
			double_jump = false;

			if (!god_mode)
			{

				if (going_right)
				{
					CurrentAnimation = deathRight;
				}
				else
				{
					CurrentAnimation = deathLeft;
				}

				if (CurrentAnimation->Finished())
				{
					if(!dead)
					dead = true;
					App->LoadGame("save_game.xml");
				}
				App->audio->PlayFx(App->audio->deathfx,0);
			}
		}

		else if (c2->type == COLLIDER_PLATFORM && dead == false)
		{
			colliding_roof = false;

			if ((going_left || going_right) && must_fall)
			{

				if(c1->rect.x + c1->rect.w >= c2->rect.x && c1->rect.x + c1->rect.w <= c2->rect.x + initialVx)
				{
					Velocity.x = 0.0f;
			     	c1->rect.x = c2->rect.x - c1->rect.w - colliding_offset;
				}

				if (c1->rect.x >= c2->rect.x + c2->rect.w - initialVx && c1->rect.x <= c2->rect.x + c2->rect.w) 
				{                                                                                                   
					Velocity.x = 0.0f;                                                                               
					c1->rect.x = c2->rect.x + c2->rect.w + colliding_offset;
				}

				if ((c1->rect.y + c1->rect.h >= c2->rect.y && c1->rect.y + c1->rect.h <= c2->rect.y + (-gravity*8))) 
				{

					if (stateplayer != JUMPING)
					{
						Velocity.y = 0.0f;
						stateplayer = IDLE;
					}

					c1->rect.y = c2->rect.y - c1->rect.h;
					double_jump = true;
					must_fall = false;
				}
				
			}

			else 
			{
				if ((c1->rect.y + c1->rect.h >= c2->rect.y && c1->rect.y + c1->rect.h <= c2->rect.y + (-gravity*8)))   
				{

					if (stateplayer != JUMPING)
					{
						Velocity.y = 0.0f;
						stateplayer = IDLE;
					}

					c1->rect.y = c2->rect.y - c1->rect.h;
					double_jump = true;
					must_fall = false;
				}

			}
			
		}
		
		else if (c2->type == COLLIDER_ROOF)
		{
			colliding_roof = true;

			if (c1->rect.y<= c2->rect.y+c2->rect.h && c1->rect.y >= c2->rect.y+c2->rect.h-initialVx+1)
			{
				c1->rect.y = c2->rect.y + c2->rect.h + colliding_offset;
				Velocity.y = 0.0f;
				stateplayer = FALLING;
				double_jump = false;
				must_fall = true;
			}
			else
			{

			  if ((stateplayer == JUMPING || stateplayer == FALLING) && going_right || going_left)
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
			if(going_right)
			{ 
				if (c1->rect.x >= c2->rect.x + c2->rect.w - colliding_offset)
				{
					App->SaveGame("save_game.xml");
			    }
			
			}
			else
			{
				if (c1->rect.x + c1->rect.w  <= c2->rect.x + colliding_offset)
				{
					App->SaveGame("save_game.xml");
				}
			}
		}


		if (first_move)
			initialmoment = false;

		if (initialmoment && !first_move)
		{
			CurrentAnimation = idleRight;
		}



		pos.x = c1->rect.x;
		pos.y = c1->rect.y;
	
	playercolliding = true;
}

bool j1Player::Load(pugi::xml_node &config)
{

	bool ret = true;

	pos.x= config.child("Playerx").attribute("value").as_float();
	pos.y = config.child("Playery").attribute("value").as_float();

	return ret;
}

bool j1Player::Save(pugi::xml_node &config) const
{
	config.append_child("Playerx").append_attribute("value")= pos.x;
	config.append_child("Playery").append_attribute("value")= pos.y;

	return true;
}

Animation* j1Player::LoadAnimation(const char* animationPath, const char* animationName) {

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

bool j1Player::CleanUp()
{
	bool ret = true;
	App->tex->UnLoad(spritesheet);

	if(playercollider != nullptr)
	playercollider = nullptr;

	return ret;
}