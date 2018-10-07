#include "j1Player.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Textures.h"
#include "j1Collision.h"
#include "j1Render.h"
#include "j1Input.h"
#include "j1Map.h"


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

	playerpos.create(0, 0);

	/*if (graphics == nullptr)
		graphics=App->tex->Load("textures/jump outline.png");*/

	if (playercollider == nullptr)
		playercollider = App->coll->AddCollider({ 0, 0, 19, 36 }, COLLIDER_PLAYER, this);
	else
		playercollider->SetPos(0, 0);


	return true;
}

bool j1Player::Update(float dt)
{

	if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT)
	{
		pos.x -= 5;
	}

	if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT)
	{
		pos.x += 5;
	}

	if (App->input->GetKey(SDL_SCANCODE_W) == KEY_DOWN && jumping==false)
	{
		jumping = true;
		firstime = true;
	}

	if (jumping == true)
	{
		currentime=SDL_GetTicks();
		/*speed -= gravity;
		pos.y -= speed;*/


		if (firstime)
		{
			firstime = false;
			lastTime = currentime;
		}

		if (currentime > lastTime + jumptime)
		{
			jumping = false;

		}

	}


	return true;
}

bool j1Player::PostUpdate()
{
	bool ret = true;


	App->render->DrawQuad(playercollider->rect, 255, 0, 0);

	playercollider->SetPos(pos.x, pos.y);


	if(ret==false)
	LOG("Player is not blitted %s", ret);

	return ret;
}

void j1Player::OnCollision(Collider * c1, Collider * c2)
{
}

bool j1Player::Load(pugi::xml_node &)
{
	return true;
}

bool j1Player::Save(pugi::xml_node &) const
{
	return true;
}
