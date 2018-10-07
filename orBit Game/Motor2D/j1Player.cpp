#include "j1Player.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Textures.h"
#include "j1Collision.h"
#include "j1Render.h"


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

	playerpos.create(0, 0);

	if (graphics == nullptr)
		App->tex->Load("textures/jump outline.png");

	if (playercollider == nullptr)
		playercollider = App->coll->AddCollider({ 0, 0, 19, 36 }, COLLIDER_PLAYER, this);
	else
		playercollider->SetPos(0, 0);


	return true;
}

bool j1Player::Update()
{
	return true;
}

bool j1Player::PostUpdate()
{
	App->render->Blit(graphics, playerpos.x, playerpos.y, &playercollider->rect);

	return true;
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
