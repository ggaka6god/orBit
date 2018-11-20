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
	entitycoll = App->coll->AddCollider(entitycollrect, COLLIDER_PLAYER, (j1Module*) manager);

	// --- Default gravity ---
	gravity = -1.0f;

	// --- Current Player Position ---
	position.x = 550;
	position.y = 150;

	entitycoll->SetPos(position.x, position.y);

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

	return true;
}

void j1Player::UpdateEntityMovement(float dt)
{

	switch (EntityMovement)
	{
		case MOVEMENT::RIGHTWARDS:
			Future_position.x += (int) 100 * dt;
			break;
		case MOVEMENT::LEFTWARDS:
			Future_position.x -= (int) 100 * dt;
			break;
		case MOVEMENT::UPWARDS:
			Future_position.y -= (int) 100 * dt;
			break;
		case MOVEMENT::FREEFALL:
			Future_position.y += (int)20 * dt;
	}

	App->coll->QueryCollisions(*entitycoll);

	MOVEMENT EntityMovement = MOVEMENT::STATIC;
}

bool j1Player::Update(float dt)
{
	// --- LOGIC --------------------

	// --- FREE FALL ---
	EntityMovement = MOVEMENT::FREEFALL;

	UpdateEntityMovement(dt);

	// --- RIGHT ---
	if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT)
	{
		EntityMovement = MOVEMENT::RIGHTWARDS;
	}

	if(EntityMovement != MOVEMENT::STATIC)
	UpdateEntityMovement(dt);

	// --- LEFT ---
	if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT)
	{
		EntityMovement = MOVEMENT::LEFTWARDS;
	}

	if (EntityMovement != MOVEMENT::STATIC)
	UpdateEntityMovement(dt);


	// --- UP ---
	if (App->input->GetKey(SDL_SCANCODE_W) == KEY_DOWN)
	{
		EntityMovement = MOVEMENT::UPWARDS;
	}

	if (EntityMovement != MOVEMENT::STATIC)
	UpdateEntityMovement(dt);

	//-------------------------------


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
	//if(going_right)
	//App->render->Blit(spritesheet, position.x - 3, position.y, &CurrentAnimation->GetCurrentFrame(dt));
	//else if (going_left)
	//App->render->Blit(spritesheet, position.x - 6, position.y, &CurrentAnimation->GetCurrentFrame(dt));
	//else
	//App->render->Blit(spritesheet, position.x - 3, position.y, &CurrentAnimation->GetCurrentFrame(dt));

	return ret;
}

void j1Player::OnCollision(Collider * c1, Collider * c2)
{
	//switch (c2->type)
	//{
	//case COLLIDER_FLOOR:

	//}
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

	EntityMovement = MOVEMENT::STATIC;

	Update(dt);

}

