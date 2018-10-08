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
	if (playercollider != nullptr)
		playercollider->to_delete = true;


	/*App->tex->UnLoad(graphics);*/
}

bool j1Player::Start()
{

	LOG("Loading player");

	/*playerpos.create(0, 0);*/

	/*if (graphics == nullptr)
		graphics=App->tex->Load("textures/jump outline.png");*/

	if (playercollider == nullptr)
		playercollider = App->coll->AddCollider({ 0, 0, 19, 36 }, COLLIDER_PLAYER, this);
	else
		playercollider->SetPos(0, 0);

	Velocity.x = 5.0f;
	Velocity.y = 30.0f;
	pos.x = 10;
	pos.y = 100;

	gravity = -9.8f;
	playercolliding = false;

	jump_force = 30.0f;
	

	return true;
}

bool j1Player::Update(float dt)
{


	//Check if player is Falling or jumping

	if (Velocity.y < 0 && stateplayer==JUMPING)
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

	if (App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN && playercolliding==true)
	{
		Velocity.y = jump_force;
		stateplayer = JUMPING;
		playercolliding = false;
	}

	if (stateplayer == JUMPING)
	{
		Velocity.y += gravity/2;
		pos.y -= Velocity.y;
	}

	if (stateplayer == FALLING )
	{
		Velocity.y += gravity/2;
		pos.y -= Velocity.y;
	}

	if (playercolliding==true)
	{
		Velocity.y = 0.0f;
	}

	return true;
}

bool j1Player::PostUpdate()
{
	bool ret = true;
	

	playercollider->SetPos(pos.x, pos.y-3);

	App->render->DrawQuad(playercollider->rect, 255, 0, 0);

	return ret;
}

void j1Player::OnCollision(Collider * c1, Collider * c2)
{
	if (c1->type == COLLIDER_FLOOR || c2->type == COLLIDER_FLOOR)
	{

		if (playercolliding == false)
		{
			if (c1->type == COLLIDER_FLOOR)
				pos.y = c1->rect.y - playercollider->rect.h;
			else
				pos.y = c2->rect.y - playercollider->rect.h;
		}

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
