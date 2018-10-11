#include "j1Collision.h"
#include "j1App.h"
#include "j1Input.h"
#include "j1Render.h"
#include "j1Player.h"
#include "p2Log.h"
#include "j1Window.h"


j1Collision::j1Collision()
{
	name.create("collision");

	matrix[COLLIDER_FLOOR][COLLIDER_PLAYER] = false;
	matrix[COLLIDER_FLOOR][COLLIDER_FLOOR] = false;

	matrix[COLLIDER_PLAYER][COLLIDER_FLOOR] = true;
	matrix[COLLIDER_PLAYER][COLLIDER_PLAYER] = false;


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

	playertouched = 0;

	bool skipcolliders = true; //skip colliders that are not in camera

	// Calculate collisions

	p2List_item <Collider*> *collider1;
	p2List_item <Collider*> *collider2;

	collider2 = collider1 = colliders.start;
	
	if(collider1->next!=NULL)
	collider2 = collider1->next;

	while(collider1!=NULL && collider2!=NULL && collider1!=collider2)
	{

		if ((collider1->data->rect.x + collider1->data->rect.w >= -App->render->camera.x 
			&& collider1->data->rect.x*App->win->GetScale() <=-App->render->camera.x + App->render->camera.w)
			&& (collider2->data->rect.x + collider2->data->rect.w >= -App->render->camera.x 
			&& collider2->data->rect.x*App->win->GetScale() <= -App->render->camera.x + App->render->camera.w))
		{
			skipcolliders = false;
		}

		while (collider2 != NULL && skipcolliders==false)
		{
			//We skip colliders that are not in camera
			skipcolliders = true;

			if (collider2->data->rect.x + collider2->data->rect.w >= -App->render->camera.x 
				&& collider2->data->rect.x*App->win->GetScale() <= -App->render->camera.x + App->render->camera.w)
			{
				skipcolliders = false;
			}

			if (collider1->data->CheckCollision(collider2->data->rect) == true && skipcolliders==false)
			{
				/*if (collider1->data->type == COLLIDER_PLAYER || collider2->data->type == COLLIDER_PLAYER)
				{
					playertouched++;
				}
                */
				if (matrix[collider1->data->type][collider2->data->type] && collider1->data->callback)
				{
					collider1->data->callback->OnCollision(collider1->data, collider2->data);
				}

				if (matrix[collider2->data->type][collider1->data->type] && collider2->data->callback)
				{
					collider2->data->callback->OnCollision(collider2->data, collider1->data);
				}
			}
			collider2 = collider2->next;
			skipcolliders = false;
		}

		if (skipcolliders == true)
		{
			collider2 = collider2->next;
			continue;
		}
		skipcolliders = true;
		collider1 = collider1->next;
		collider2 = collider1->next;
	}
	//if (playertouched == 0) 
	//{
	//	LOG("playertouched is %i", playertouched);
	//	//App->player->playercolliding = false;

	//	if(App->player->stateplayer==IDLE)
	//	App->player->stateplayer = FALLING;
	//
	//}
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
		case COLLIDER_FLOOR: // blue
			App->render->DrawQuad(item->data->rect, 0, 0, 255, alpha);
			break;
		case COLLIDER_PLAYER: // green
			App->render->DrawQuad(item->data->rect, 0, 255, 0, alpha);
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
