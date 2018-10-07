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
	if (playercollider != nullptr)
		playercollider->to_delete = true;


	App->tex->UnLoad(graphics);
}

bool j1Player::Start()
{

	LOG("Loading player");

	playerpos.create(0, 0);

	if (graphics == nullptr)
		graphics=App->tex->Load("textures/jump outline.png");

	if (playercollider == nullptr)
		playercollider = App->coll->AddCollider({ 0, 0, 19, 36 }, COLLIDER_PLAYER, this);
	else
		playercollider->SetPos(0, 0);


	return true;
}

bool j1Player::Update()
{

	playercollider->SetPos(playerpos.x, playerpos.y);
	return true;
}

bool j1Player::PostUpdate()
{
	bool ret = true;

	ret=App->render->Blit(graphics, playerpos.x, playerpos.y, &playercollider->rect);
	/*playercollider->SetPos(playerpos.x, playerpos.y);*/


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
