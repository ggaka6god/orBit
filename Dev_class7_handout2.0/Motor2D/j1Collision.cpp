#include "j1Collision.h"
#include "j1App.h"
#include "j1Input.h"
#include "j1Render.h"
#include "j1Player.h"
#include "p2Log.h"


j1Collision::j1Collision()
{
	name.create("collision");

	matrix[COLLIDER_FLOOR][COLLIDER_PLAYER] = false;
	matrix[COLLIDER_FLOOR][COLLIDER_FLOOR] = false;

	matrix[COLLIDER_PLAYER][COLLIDER_FLOOR] = true;
	matrix[COLLIDER_PLAYER][COLLIDER_PLAYER] = false;

	matrix[COLLIDER_PLAYER][COLLIDER_SPIKES] = true;
	matrix[COLLIDER_PLAYER][COLLIDER_PLATFORM] = true;

}

j1Collision::~j1Collision()
{
}

bool j1Collision::PreUpdate()
{

	bool ret = true;

	// Remove all colliders scheduled for deletion

	p2List_item <Collider*> *item;
	item = colliders.start;

	while (item != NULL)
	{
		if(item->data->to_delete==true)
		RELEASE(item->data);

		item = item->next;
	}


	return ret;
}

bool j1Collision::Update(float dt)
{

	bool ret = true;

	return ret;
}

bool j1Collision::PostUpdate()
{
	DebugDraw();

	return true;
}


bool j1Collision::CleanUp()
{
	LOG("Freeing all colliders");

	p2List_item <Collider*> *item;
	item = colliders.start;

	while (item != NULL)
	{
		RELEASE(item->data);
		item = item->next;
	}
	colliders.clear();


	return true;
}

Collider * j1Collision::AddCollider(SDL_Rect rect, COLLIDER_TYPE type, j1Module * callback)
{

	Collider * to_Add = new Collider(rect, type, callback);

	colliders.add(to_Add);

	return to_Add;
}

void j1Collision::DebugDraw()
{

	if (App->input->GetKey(SDL_SCANCODE_F1) == KEY_DOWN)
		debug = !debug;

	if (debug == false)
		return;

	p2List_item <Collider*> *item;
	item = colliders.start;

	Uint8 alpha = 80;

	while (item!=NULL)
	{

		switch (item->data->type)
		{
		case COLLIDER_NONE: // white
			App->render->DrawQuad(item->data->rect, 255, 255, 255, alpha);
			break;
		case COLLIDER_FLOOR: // red
			App->render->DrawQuad(item->data->rect, 255, 0, 0, alpha);
			break;
		case COLLIDER_PLAYER: // green
			App->render->DrawQuad(item->data->rect, 0, 255, 0, alpha);
			break;
		case COLLIDER_SPIKES: // yellow
			App->render->DrawQuad(item->data->rect, 255, 255, 0, alpha);
			break;
		case COLLIDER_PLATFORM: // green
			App->render->DrawQuad(item->data->rect, 255, 0, 255, alpha);
			break;
			

		}
		item = item->next;
	}

}


bool Collider::CheckCollision(const SDL_Rect & r) const
{
	return (rect.x < r.x + r.w &&
		rect.x + rect.w > r.x &&
		rect.y < r.y + r.h &&
		rect.h + rect.y > r.y);

	// between argument "r" and property "rect"
}
