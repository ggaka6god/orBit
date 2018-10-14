#include "j1Player.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Textures.h"
#include "j1Collision.h"
#include "j1Render.h"
#include "j1Input.h"
#include "j1Map.h"
#include "j1Scene.h"

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
	fallingRight = LoadAnimation(folder.GetString(), "air righr");
	fallingLeft = LoadAnimation(folder.GetString(), "air left");
	deathRight = LoadAnimation(folder.GetString(), "dead right");
	deathLeft = LoadAnimation(folder.GetString(), "dead left");
	airRight = LoadAnimation(folder.GetString(), "air right");
	airLeft = LoadAnimation(folder.GetString(), "air left");
	int x = config.child("colider").attribute("x").as_int();
	int y = config.child("colider").attribute("y").as_int();
	int w = config.child("colider").attribute("width").as_int();
	int h = config.child("colider").attribute("height").as_int();
	playercol = { x,y,w,h };
	
	/*initpos1.x = config.child("stg1InitPos").attribute("x").as_int();
	initpos1.y = config.child("stg1InitPos").attribute("y").as_int();
	initpos2.x = config.child("stg2InitPos").attribute("x").as_int();
	initpos2.y = config.child("stg2InitPos").attribute("y").as_int()*/;

	runRight->speed = 0.15f;
	runLeft->speed = 0.15f;

	/*CurrentAnimation = airRight;*/

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

	Velocity.x = 2.0f;
	Velocity.y = 0.0f;
	

	gravity = -1.0f;
	playercolliding = false;

	jump_force = 7.5f;
	max_speed_y = 10.0f;
	stateplayer = IDLE;
	must_fall = false;
	double_jump = true;

	if (spritesheet == nullptr)
		spritesheet = App->tex->Load(Texture.GetString());

	return true;
}

bool j1Player::Update(float dt)
{
	if (wasRight==true)
		CurrentAnimation = idleRight;
	else if (wasRight==false)
		CurrentAnimation = idleLeft;

	//Check if player is Falling or jumping

	/*if (must_fall)
	{
		playercolliding = false;
	}*/

	if (Velocity.y < 0 && stateplayer == JUMPING)
	{
		stateplayer = FALLING;
	}

	if (playercolliding == false && stateplayer == IDLE)
	{
		stateplayer = FALLING;
	}

	//Horizontal Movement 

	

	if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT)
	{
		Velocity.x = 2.0f;
		pos.x = pos.x - Velocity.x;
		going_left = true;
		going_right = false;
		CurrentAnimation = runLeft;
		wasRight = false;
	}

	if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT)
	{
		Velocity.x = 2.0f;
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
	}

	//Vertical Movement


	if (!must_fall)
	{
		if (App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN && playercolliding == true)
		{
			Velocity.y = jump_force;
			stateplayer = JUMPING;
			playercolliding = false;
			
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
		if (double_jump == true && App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN && Velocity.y!=jump_force)
		{
			Velocity.y = jump_force/1.5f;
			pos.y -= Velocity.y;
			double_jump = false;
		}
	
		Velocity.y += gravity / 2;
		pos.y -= Velocity.y;

	}

	if (stateplayer == FALLING && !colliding_roof)
	{
		must_fall = false;

		if (going_right)
		{
			CurrentAnimation = fallingRight;
		}
		else
		{
			CurrentAnimation = fallingLeft;
		}

		if (double_jump == true && App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN && Velocity.y != jump_force)
		{
			/*stateplayer = JUMPING;*/
			Velocity.y = jump_force/1.5f;
			pos.y -= Velocity.y;
			double_jump = false;
		}

		Velocity.y += gravity / 2;
		pos.y -= Velocity.y;

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
	/*	if(going_right)
		CurrentAnimation = airRight;
		if(going_left)
		CurrentAnimation = airLeft;*/
	}

	return true;
}

bool j1Player::PostUpdate()
{
	bool ret = true;

	App->render->Blit(spritesheet, pos.x, pos.y, &CurrentAnimation->GetCurrentFrame());

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

	if (c2->type == COLLIDER_FLOOR)
	{
		colliding_roof = false;
	
			if ((going_left || going_right) && must_fall)
			{

				if (c1->rect.x + c1->rect.w >= c2->rect.x && c1->rect.x + c1->rect.w <= c2->rect.x + 3)
				{
					Velocity.x = 0.0f;
					c1->rect.x = c2->rect.x - c1->rect.w - 0.1f;
				}

				if (c1->rect.x >= c2->rect.x + c2->rect.w - 3 && c1->rect.x <= c2->rect.x + c2->rect.w)
				{
					Velocity.x = 0.0f;
					c1->rect.x = c2->rect.x + c2->rect.w + 0.1f;
				}

				if (lateralcollision == true)
				{

					if (going_left)
						c1->rect.x += 1.0f;
					else
						c1->rect.x -= 1.0f;

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
				// c2 ==COLLIDER_FLOOR 

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

					//stopping player if lateral col0lision
					if (lateralcollision)
					{
						if (c1->rect.x + c1->rect.w >= c2->rect.x && c1->rect.x + c1->rect.w <= c2->rect.x + 3)
						{
							Velocity.x = 0.0f;
							if (stateplayer != JUMPING)
								c1->rect.y = aux;
							c1->rect.x = c2->rect.x - c1->rect.w;
						}

						if (stateplayer == JUMPING || stateplayer == FALLING && double_jump)
						{
							c1->rect.x -= 1.0f;
						}
					}
					else if (!lateralcollision && must_fall == false)
						stateplayer = IDLE;

					if ((going_left || going_right) && must_fall)
					{
						c1->rect.x = c2->rect.x + c2->rect.w - 1;
					}
				}
				//going left
				if (going_left)
				{
					if (lateralcollision)
					{

						if (c1->rect.x >= c2->rect.x + c2->rect.w - 3 && c1->rect.x <= c2->rect.x + c2->rect.w)
						{
							Velocity.x = 0.0f;
							if (stateplayer != JUMPING)
								c1->rect.y = aux;
							c1->rect.x = c2->rect.x + c2->rect.w;
						}

						if (stateplayer == JUMPING || stateplayer == FALLING && double_jump)
						{
							c1->rect.x += 1.0f;
						}
					}
					else if (!lateralcollision && must_fall == false)
						stateplayer = IDLE;

					if ((going_left || going_right) && must_fall)
					{
						c1->rect.x = c2->rect.x + c2->rect.w + 1;
					}
				}

				double_jump = true;
				must_fall = false;

			}
		}

		else if (c2->type == COLLIDER_SPIKES)
		{
			colliding_roof = false;
			/*Velocity.y = 0;*/
			must_fall = false;
			double_jump = false;

		/*	if (going_right)
			{
				CurrectAnimation = deathRight;
			}
			else
			{
				CurrectAnimation = deathRight;
			}*/

		}

		else if (c2->type == COLLIDER_PLATFORM)
		{
			colliding_roof = false;

			if ((going_left || going_right) && must_fall)
			{
				if(c1->rect.x + c1->rect.w >= c2->rect.x && c1->rect.x + c1->rect.w <= c2->rect.x + 3)
				{
					Velocity.x = 0.0f;
			     	c1->rect.x = c2->rect.x - c1->rect.w-1.0f;
				}

				if (c1->rect.x >= c2->rect.x + c2->rect.w - 3 && c1->rect.x <= c2->rect.x + c2->rect.w) 
				{                                                                                                   
					Velocity.x = 0.0f;                                                                               
					c1->rect.x = c2->rect.x + c2->rect.w+1.0f;
				}

				if ((c1->rect.y + c1->rect.h >= c2->rect.y && c1->rect.y + c1->rect.h <= c2->rect.y + 8))
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
				if ((c1->rect.y + c1->rect.h >= c2->rect.y && c1->rect.y + c1->rect.h <= c2->rect.y + 8))
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

			if (c1->rect.y<= c2->rect.y+c2->rect.h && c1->rect.y >= c2->rect.y+c2->rect.h-3)
			{
				c1->rect.y = c2->rect.y + c2->rect.h + 1;
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
					c1->rect.x = c2->rect.x - c1->rect.w-1;
					
				}
				else
				{
					c1->rect.x = c2->rect.x + c2->rect.w+1;
				}
			}
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

	return ret;
}