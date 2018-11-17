#include "j1Collision.h"
#include "j1App.h"
#include "j1Input.h"
#include "j1Render.h"
#include "j1Player.h"
#include "p2Log.h"
#include "j1Window.h"
#include "j1Scene.h"


j1Collision::j1Collision()
{
	name.create("collision");

	matrix[COLLIDER_FLOOR][COLLIDER_PLAYER] = false;
	matrix[COLLIDER_FLOOR][COLLIDER_FLOOR] = false;
	matrix[COLLIDER_FLOOR][COLLIDER_SPIKES] = false;
	matrix[COLLIDER_FLOOR][COLLIDER_PLATFORM] = false;
	matrix[COLLIDER_FLOOR][COLLIDER_ROOF] = false;
	matrix[COLLIDER_FLOOR][CHECKPOINT] = false;
	matrix[COLLIDER_FLOOR][COLLIDER_ENEMY_BAT] = false;
	matrix[COLLIDER_FLOOR][COLLIDER_ENEMY_SLIME] = false;

	matrix[COLLIDER_PLATFORM][COLLIDER_PLAYER] = false;
	matrix[COLLIDER_PLATFORM][COLLIDER_FLOOR] = false;
	matrix[COLLIDER_PLATFORM][COLLIDER_SPIKES] = false;
	matrix[COLLIDER_PLATFORM][COLLIDER_PLATFORM] = false;
	matrix[COLLIDER_PLATFORM][COLLIDER_ROOF] = false;
	matrix[COLLIDER_PLATFORM][CHECKPOINT] = false;
	matrix[COLLIDER_PLATFORM][COLLIDER_ENEMY_BAT] = false;
	matrix[COLLIDER_PLATFORM][COLLIDER_ENEMY_SLIME] = false;

	matrix[COLLIDER_ROOF][COLLIDER_PLAYER] = false;
	matrix[COLLIDER_ROOF][COLLIDER_FLOOR] = false;
	matrix[COLLIDER_ROOF][COLLIDER_SPIKES] = false;
	matrix[COLLIDER_ROOF][COLLIDER_PLATFORM] = false;
	matrix[COLLIDER_ROOF][COLLIDER_ROOF] = false;
	matrix[COLLIDER_ROOF][CHECKPOINT] = false;
	matrix[COLLIDER_ROOF][COLLIDER_ENEMY_BAT] = false;
	matrix[COLLIDER_ROOF][COLLIDER_ENEMY_SLIME] = false;

	matrix[CHECKPOINT][COLLIDER_PLAYER] = false;
	matrix[CHECKPOINT][COLLIDER_FLOOR] = false;
	matrix[CHECKPOINT][COLLIDER_SPIKES] = false;
	matrix[CHECKPOINT][COLLIDER_PLATFORM] = false;
	matrix[CHECKPOINT][COLLIDER_ROOF] = false;
	matrix[CHECKPOINT][CHECKPOINT] = false;
	matrix[CHECKPOINT][COLLIDER_ENEMY_BAT] = false;
	matrix[CHECKPOINT][COLLIDER_ENEMY_SLIME] = false;

	matrix[COLLIDER_SPIKES][COLLIDER_PLAYER] = false;
	matrix[COLLIDER_SPIKES][COLLIDER_FLOOR] = false;
	matrix[COLLIDER_SPIKES][COLLIDER_SPIKES] = false;
	matrix[COLLIDER_SPIKES][COLLIDER_PLATFORM] = false;
	matrix[COLLIDER_SPIKES][COLLIDER_ROOF] = false;
	matrix[COLLIDER_SPIKES][CHECKPOINT] = false;
	matrix[COLLIDER_SPIKES][COLLIDER_ENEMY_BAT] = false;
	matrix[COLLIDER_SPIKES][COLLIDER_ENEMY_SLIME] = false;

	matrix[COLLIDER_PLAYER][COLLIDER_FLOOR] = true;
	matrix[COLLIDER_PLAYER][COLLIDER_PLAYER] = false;
	matrix[COLLIDER_PLAYER][COLLIDER_SPIKES] = true;
	matrix[COLLIDER_PLAYER][COLLIDER_PLATFORM] = true;
	matrix[COLLIDER_PLAYER][COLLIDER_ROOF] = true;
	matrix[COLLIDER_PLAYER][CHECKPOINT] = true;
    matrix[COLLIDER_PLAYER][COLLIDER_ENEMY_SLIME] = true;
	matrix[COLLIDER_PLAYER][COLLIDER_ENEMY_BAT] = true;

	matrix[COLLIDER_ENEMY_SLIME][COLLIDER_FLOOR] = true;
	matrix[COLLIDER_ENEMY_SLIME][COLLIDER_PLATFORM] = true;
	matrix[COLLIDER_ENEMY_SLIME][CHECKPOINT] = false;
	matrix[COLLIDER_ENEMY_SLIME][COLLIDER_SPIKES] = false;
	matrix[COLLIDER_ENEMY_SLIME][COLLIDER_ROOF] = false;
	matrix[COLLIDER_ENEMY_SLIME][COLLIDER_ENEMY_BAT] = false;
	matrix[COLLIDER_ENEMY_SLIME][COLLIDER_PLAYER] = false;
	matrix[COLLIDER_ENEMY_SLIME][COLLIDER_ENEMY_SLIME] = false;

	matrix[COLLIDER_ENEMY_BAT][COLLIDER_FLOOR] = true;
	matrix[COLLIDER_ENEMY_BAT][COLLIDER_SPIKES] = true;
	matrix[COLLIDER_ENEMY_BAT][COLLIDER_PLATFORM] = true;
	matrix[COLLIDER_ENEMY_BAT][COLLIDER_ROOF] = true;
	matrix[COLLIDER_ENEMY_BAT][CHECKPOINT] = false;
	matrix[COLLIDER_ENEMY_BAT][COLLIDER_ENEMY_BAT] = false;
	matrix[COLLIDER_ENEMY_BAT][COLLIDER_ENEMY_SLIME] = false;
	matrix[COLLIDER_ENEMY_BAT][COLLIDER_PLAYER] = false;

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
		if (item->data->to_delete == true)
			colliders.del(item);

		item = item->next;
	}


	return ret;
}

bool j1Collision::Update(float dt)
{

	bool ret = true;

	playertouched = 0;

	bool skipcolliders = true; //skip colliders that are not entities

	// Calculate collisions

	p2List_item <Collider*> *collider1;
	p2List_item <Collider*> *collider2;

	collider2 = collider1 = colliders.start;
	
	if(collider1->next!=NULL)
	collider2 = collider1->next;

	

	while(collider1!=NULL && collider2!=NULL && collider1!=collider2)
	{
		skipcolliders = true;

		//check for entities in colliders
		if (collider1->data->type == COLLIDER_PLAYER	  || collider2->data->type == COLLIDER_PLAYER      ||
			collider1->data->type == COLLIDER_ENEMY_SLIME || collider2->data->type == COLLIDER_ENEMY_SLIME ||
			collider1->data->type == COLLIDER_ENEMY_BAT   || collider2->data->type == COLLIDER_ENEMY_BAT)
		{
			skipcolliders = false;
		}
		

		while (collider2 != NULL && skipcolliders==false)
		{
			//We skip colliders that are not in camera
			skipcolliders = true;
			
			//only check area near entity
			if ( // Target Collision    ------------------------------   Set Area surrounding Entity
				(collider2->data->rect.x							  <= collider1->data->rect.x + App->scene->areaofcollision &&
				 collider2->data->rect.x + collider2->data->rect.w    >= collider1->data->rect.x - App->scene->areaofcollision &&
				 collider2->data->rect.y							  <= collider1->data->rect.y + App->scene->areaofcollision &&
				 collider2->data->rect.y + collider2->data->rect.h    >= collider1->data->rect.y - App->scene->areaofcollision)
															||
				(collider1->data->rect.x							  <= collider2->data->rect.x + App->scene->areaofcollision &&
				 collider1->data->rect.x + collider1->data->rect.w	  >= collider2->data->rect.x - App->scene->areaofcollision &&
				 collider1->data->rect.y							  <= collider2->data->rect.y + App->scene->areaofcollision &&
				 collider1->data->rect.y + collider1->data->rect.h	  >= collider2->data->rect.y - App->scene->areaofcollision)
				)
			{
				skipcolliders = false;
			}
			
			if (collider1->data->CheckCollision(collider2->data->rect) == true && skipcolliders == false)
			{
					if (collider1->data->type == COLLIDER_PLAYER || collider2->data->type == COLLIDER_PLAYER)
					{
						playertouched++;
					}

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

		collider1 = collider1->next;
		collider2 = collider1->next;
	}
	

	if (App->scene->player->entitystate!=JUMPING && App->scene->player->entitystate!=FALLING && playertouched == 0) 
	{
		App->scene->player->must_fall = true;
	}

	return ret;
}

bool j1Collision::PostUpdate(float dt)
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

	if (App->input->GetKey(SDL_SCANCODE_F9) == KEY_DOWN) //collider draw
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
		case COLLIDER_PLATFORM: // magenta
			App->render->DrawQuad(item->data->rect, 255, 0, 255, alpha);
			break;
		case COLLIDER_ROOF: // rose
			App->render->DrawQuad(item->data->rect, 255, 0, 128, alpha);
			break;
		case CHECKPOINT: // blue
			App->render->DrawQuad(item->data->rect, 0 , 0, 128, alpha);
			break;
		case COLLIDER_ENEMY_SLIME: // brown
			App->render->DrawQuad(item->data->rect, 153, 76, 0, alpha);
			break;
		case COLLIDER_ENEMY_BAT: // dark red
			App->render->DrawQuad(item->data->rect, 153, 0, 76, alpha);
			break;
			

		}
		item = item->next;
	}

}


bool Collider::CheckCollision(const SDL_Rect & r) const
{
	return (rect.x <= r.x + r.w &&
		rect.x + rect.w >= r.x &&
		rect.y <= r.y + r.h &&
		rect.h + rect.y >= r.y);

	// between argument "r" and property "rect"
}
