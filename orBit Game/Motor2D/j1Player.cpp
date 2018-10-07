#include "j1Player.h"
#include "p2Log.h"

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

	return true;
}

bool j1Player::Update()
{
	return true;
}

bool j1Player::PostUpdate()
{
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
