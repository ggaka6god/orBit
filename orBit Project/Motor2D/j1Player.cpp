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
#include "j1EntityManager.h"


j1Player::j1Player() : j1Entity("player",entity_type::PLAYER)
{
}

j1Player::~j1Player()
{

}

bool j1Player::Start()
{
	LOG("--- Loading player ---");

	// --- Get every data loaded by Entity Manager ---
	playerinfo = manager->GetPlayerData();

	// --- Player's bounding box ---
	entitycollrect = playerinfo.playerrect;
	entitycoll = App->coll->AddCollider(entitycollrect,COLLIDER_TYPE::COLLIDER_PLAYER, (j1Module*) manager);

	// --- Default gravity ---
	gravity = 29.4f;

	// --- Current Player Position ---
	position.x = 550;
	position.y = 150;

	entitycoll->SetPos(position.x, position.y);

	Future_position.x = position.x;
	Future_position.y = position.y;

	// --- Parallax Movement variables ---
	parallaxflow = 0;
	previousflow = 0;

	// --- Currently playing Animation ---
	CurrentAnimation = playerinfo.idleRight;
	
	// --- Entity Spritesheet ---
	if (spritesheet == nullptr)
		spritesheet = App->tex->Load(playerinfo.Texture.GetString());

	// --- Entity ID for save purposes ---
	entityID = App->entities->entityID;
	
	Velocity.x = 35.0f;
	Velocity.y = 0.0f;

	playerinfo.jump_force = 350.0f;

	Accumulative_pos_Right =  0;

	return true;
}

void j1Player::UpdateEntityMovement(float dt)
{

	switch (EntityMovement)
	{
		case MOVEMENT::RIGHTWARDS:
			Accumulative_pos_Right += Velocity.x*dt;

			if (Accumulative_pos_Right > 1.0f)
			{
				Future_position.x += Accumulative_pos_Right;
				Accumulative_pos_Right -= Accumulative_pos_Right;
			}
			break;
		case MOVEMENT::LEFTWARDS:
			Accumulative_pos_Left += (Velocity.x)*dt;

			if (on_air)
			{
				if (Accumulative_pos_Left > 1.0f)
				{
					Future_position.x -= Accumulative_pos_Left;
					Future_position.x -= Accumulative_pos_Left;

					Accumulative_pos_Left -= Accumulative_pos_Left;
				}
			}
			else
				if (Accumulative_pos_Left > 0.75f)
				{
					Future_position.x -= Accumulative_pos_Left;

					Accumulative_pos_Left -= Accumulative_pos_Left;
				}
			break;
		case MOVEMENT::UPWARDS:

			Accumulative_pos_Up += Velocity.y*dt;

			if (Accumulative_pos_Up > 1.0f)
			{
				Future_position.y -= Accumulative_pos_Up;
				Accumulative_pos_Up -= Accumulative_pos_Up;
			}
			break;
		case MOVEMENT::FREEFALL:

			Accumulative_pos_Down += gravity* dt;

			if(on_air)
			Accumulative_pos_Down += gravity * dt;

			if (Accumulative_pos_Down > 1.0f)
			{
				Velocity.y -= Accumulative_pos_Down;
				Future_position.y += Accumulative_pos_Down;
				Accumulative_pos_Down -= Accumulative_pos_Down;
			}
			
			break;
	}

	entitycoll->SetPos(Future_position.x, Future_position.y);

	App->coll->QueryCollisions(*entitycoll);

	MOVEMENT EntityMovement = MOVEMENT::STATIC;
}

inline void j1Player::Apply_Vertical_Impulse(float dt)
{
	Velocity.y += playerinfo.jump_force;
}

void j1Player::Handle_Animations()
{
	// --- Handling Ground Animations ---

	if (!on_air)
	{
		//--- TO RUN ---

		if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT && App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT)
		{
			if (CurrentAnimation == playerinfo.runRight)
				CurrentAnimation = playerinfo.idleRight;
			else if (CurrentAnimation == playerinfo.runLeft)
				CurrentAnimation = playerinfo.idleLeft;
		}
		else
		{

			if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT &&
				CurrentAnimation != playerinfo.jumpingRight      &&
				CurrentAnimation != playerinfo.fallingRight)
			{
				CurrentAnimation = playerinfo.runRight;
			}

			if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT &&
				CurrentAnimation != playerinfo.jumpingLeft       &&
				CurrentAnimation != playerinfo.fallingLeft)
			{
				CurrentAnimation = playerinfo.runLeft;
			}
		}

		//--- TO IDLE ---

		if (App->input->GetKey(SDL_SCANCODE_D) == KEY_UP && CurrentAnimation == playerinfo.runRight)
			CurrentAnimation = playerinfo.idleRight;

		if (CurrentAnimation == playerinfo.fallingRight)
			CurrentAnimation = playerinfo.idleRight;

		else if (App->input->GetKey(SDL_SCANCODE_A) == KEY_UP && CurrentAnimation == playerinfo.runLeft)
			CurrentAnimation = playerinfo.idleLeft;
		
		if (CurrentAnimation == playerinfo.fallingLeft)
			CurrentAnimation = playerinfo.idleLeft;

	}
    //--------------    ---------------


	// --- Handling Aerial Animations ---

	else if (on_air)
	{
		//--- TO JUMP ---

		if (last_pos_y > Future_position.y)
		{
			if (CurrentAnimation == playerinfo.runRight || CurrentAnimation == playerinfo.idleRight)
				CurrentAnimation = playerinfo.jumpingRight;

			else if (CurrentAnimation == playerinfo.runLeft || CurrentAnimation == playerinfo.idleLeft)
				CurrentAnimation = playerinfo.jumpingLeft;
		}

		//--- TO FALL ---

		if (last_pos_y < Future_position.y)
		{
			if (CurrentAnimation == playerinfo.jumpingRight)
				CurrentAnimation = playerinfo.fallingRight;
			else if (CurrentAnimation == playerinfo.jumpingLeft)
				CurrentAnimation = playerinfo.fallingLeft;
		}
	}

	//------------ --------------

}

bool j1Player::Update(float dt)
{
	// --- LOGIC --------------------

	// --- RIGHT --
	if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT &&
		App->input->GetKey(SDL_SCANCODE_A) != KEY_REPEAT)
	{
		EntityMovement = MOVEMENT::RIGHTWARDS;
	}

	if (EntityMovement != MOVEMENT::STATIC && EntityMovement != MOVEMENT::FREEFALL)
		UpdateEntityMovement(dt);

	// --- LEFT ---
	if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT &&
		App->input->GetKey(SDL_SCANCODE_D) != KEY_REPEAT)
	{
		EntityMovement = MOVEMENT::LEFTWARDS;
	}

	if (EntityMovement != MOVEMENT::STATIC && EntityMovement != MOVEMENT::FREEFALL)
		UpdateEntityMovement(dt);


	// --- IMPULSE ---
	if (!on_air && App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN)
	{
		Apply_Vertical_Impulse(dt);
		on_air = true;
	}

	// --- UP ---
	if (on_air && Velocity.y>0.0f)
	{
 		EntityMovement = MOVEMENT::UPWARDS;
	}

	if (EntityMovement != MOVEMENT::STATIC && EntityMovement != MOVEMENT::FREEFALL)
	UpdateEntityMovement(dt);

	// --- FREE FALL ---
	for (unsigned short i = 0; i < 4; ++i)
	{
		EntityMovement = MOVEMENT::FREEFALL;

		UpdateEntityMovement(dt);
	}

	//-------------------------------

	// --- Handling animations ---

	Handle_Animations();

	return true;
}

bool j1Player::PostUpdate(float dt)
{
	bool ret = true;

	// --- Parallax movement ---

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

	// ---------------------- //

	//Blitting player
	if(CurrentAnimation==playerinfo.runRight || CurrentAnimation==playerinfo.idleRight || CurrentAnimation==playerinfo.fallingRight || CurrentAnimation==playerinfo.jumpingRight)
	App->render->Blit(spritesheet, Future_position.x - 3, Future_position.y, &CurrentAnimation->GetCurrentFrame(dt));

	else if (CurrentAnimation == playerinfo.runLeft || CurrentAnimation == playerinfo.idleLeft || CurrentAnimation == playerinfo.fallingLeft || CurrentAnimation == playerinfo.jumpingLeft)
	App->render->Blit(spritesheet, Future_position.x - 6, Future_position.y, &CurrentAnimation->GetCurrentFrame(dt));

	else
	App->render->Blit(spritesheet, Future_position.x - 3, Future_position.y, &CurrentAnimation->GetCurrentFrame(dt));

	return ret;
}

void j1Player::OnCollision(Collider * entitycollider, Collider * to_check)
{
	switch (EntityMovement)
	{
		case MOVEMENT::RIGHTWARDS:
			Right_Collision(entitycollider, to_check);
			break;
		case MOVEMENT::LEFTWARDS:
			Left_Collision(entitycollider, to_check);
			break;
		case MOVEMENT::UPWARDS:
			Up_Collision(entitycollider, to_check);
			break;
		case MOVEMENT::FREEFALL:
			Down_Collision(entitycollider, to_check);
			break;
	}

	Future_position.x = entitycollider->rect.x;
	Future_position.y = entitycollider->rect.y;
}

void j1Player::Right_Collision(Collider * entitycollider, const Collider * to_check)
{
	SDL_Rect Intersection = { 0,0,0,0 };

	SDL_IntersectRect(&entitycollider->rect, &to_check->rect, &Intersection);

	switch (to_check->type)
	{
		case COLLIDER_TYPE::COLLIDER_FLOOR:
			entitycollider->rect.x -= Intersection.w;
			break;
	}

}

void j1Player::Left_Collision(Collider * entitycollider, const Collider * to_check)
{
	SDL_Rect Intersection = { 0,0,0,0 };

	SDL_IntersectRect(&entitycollider->rect, &to_check->rect, &Intersection);

	switch (to_check->type)
	{
		case COLLIDER_TYPE::COLLIDER_FLOOR:
			entitycollider->rect.x += Intersection.w;
			break;
	}
}

void j1Player::Up_Collision(Collider * entitycollider, const Collider * to_check)
{
	SDL_Rect Intersection = { 0,0,0,0 };

	SDL_IntersectRect(&entitycollider->rect, &to_check->rect, &Intersection);

	switch (to_check->type)
	{
		case COLLIDER_TYPE::COLLIDER_FLOOR:
			entitycollider->rect.y += Intersection.h;
			break;
	}
}

void j1Player::Down_Collision(Collider * entitycollider, const Collider * to_check)
{
	SDL_Rect Intersection = { 0,0,0,0 };

	SDL_IntersectRect(&entitycollider->rect, &to_check->rect, &Intersection);

	switch (to_check->type)
	{
		case COLLIDER_TYPE::COLLIDER_FLOOR:
			entitycollider->rect.y -= Intersection.h;
			break;
	}

	Velocity.y =  0.0f;
	on_air = false;
}


bool j1Player::Load(pugi::xml_node &config)
{
	position.x = config.child("Player").child("Playerx").attribute("value").as_float();
	position.y = config.child("Player").child("Playery").attribute("value").as_float();

	return true;
}

bool j1Player::Save(pugi::xml_node &config) const
{
	config.append_child("Player").append_child("Playerx").append_attribute("value")= position.x;
	config.child("Player").append_child("Playery").append_attribute("value")= position.y;

	return true;
}

bool j1Player::CleanUp()
{
	App->tex->UnLoad(spritesheet);

	if(entitycoll != nullptr)
	entitycoll = nullptr;

	return true;
}

void j1Player::FixedUpdate(float dt)
{
	// --- Update we do every frame ---

	PostUpdate(dt);

}

void j1Player::LogicUpdate(float dt)
{
	// --- Update we may not do every frame ---
	last_pos_y = Future_position.y;

	EntityMovement = MOVEMENT::STATIC;

	Update(dt);
}

