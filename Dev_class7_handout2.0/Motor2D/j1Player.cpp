#include "j1Player.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Textures.h"
#include "j1Collision.h"
#include "j1Render.h"
#include "j1Input.h"
#include "j1Map.h"
#include "j1Scene.h"


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

	playercollider = App->coll->AddCollider({ 0, 0, 19, 36 }, COLLIDER_PLAYER, this);

	Velocity.x = 3.0f;
	Velocity.y = 0.0f;
	pos.x = 10.0f;
	pos.y = 150.0f;

	gravity = -1.0f;
	playercolliding = false;

	jump_force = 10.0f;
	max_speed_y = 10.0f;
	stateplayer = IDLE;
	must_fall = false;

	return true;
}

bool j1Player::Update(float dt)
{
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

	if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT)
	{
		pos.x = pos.x - Velocity.x;
	}

	if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT)
	{
		pos.x = pos.x + Velocity.x;
	}


	//Vertical Movement

	if (App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN && playercolliding == true)
	{
		Velocity.y = jump_force;
		stateplayer = JUMPING;
		playercolliding = false;
	}

	if (stateplayer == JUMPING)
	{
		Velocity.y += gravity / 2;
		pos.y -= Velocity.y;

	}

	if (stateplayer == FALLING)
	{
		Velocity.y += gravity / 2;
		pos.y -= Velocity.y;

	}

	//Limit maximum y axis velocity
	if (Velocity.y < -max_speed_y)
		Velocity.y = -max_speed_y;


	playercollider->SetPos(pos.x, pos.y);

	App->coll->Update(1.0f);

	playercollider->SetPos(pos.x, pos.y);

	if (must_fall)
	{
		pos.y -= gravity*2.0f;
	}


	return true;
}

bool j1Player::PostUpdate()
{
	bool ret = true;

	App->render->DrawQuad(playercollider->rect, 0, 255, 0);

	return ret;
}

void j1Player::OnCollision(Collider * c1, Collider * c2)
{
	if (c1->type == COLLIDER_FLOOR || c2->type == COLLIDER_FLOOR)
	{
     		if (c1->type == COLLIDER_FLOOR)
				pos.y = c1->rect.y - c2->rect.h;
			else
				pos.y = c2->rect.y - c1->rect.h;

		Velocity.y = 0.0f;
		stateplayer = IDLE;
		playercolliding = true;
	}
}

bool j1Player::Load(pugi::xml_node &)
{
	return true;
}

bool j1Player::Save(pugi::xml_node &) const
{
	return true;
}
