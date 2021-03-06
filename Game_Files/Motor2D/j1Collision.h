#ifndef __j1Collision_H__
#define __j1Collision_H__

#include "j1Module.h"
#include "SDL/include/SDL_rect.h"
#include "p2List.h"

enum class COLLIDER_TYPE
{
	COLLIDER_NONE = -1,
	COLLIDER_FLOOR,
	COLLIDER_PLAYER,
	COLLIDER_SPIKES,
	COLLIDER_PLATFORM,
	COLLIDER_ROOF,
	CHECKPOINT,
	COLLIDER_ENEMY_SLIME,
	COLLIDER_ENEMY_BAT,
	COLLIDER_MAX
};

struct Collider
{
	SDL_Rect rect;
	bool to_delete = false;
	COLLIDER_TYPE type;
	j1Module* callback = nullptr;

	Collider(SDL_Rect rectangle, COLLIDER_TYPE type, j1Module* callback = nullptr) :
		rect(rectangle),
		type(type),
		callback(callback)
	{}

	void SetPos(int x, int y)
	{
		rect.x = x;
		rect.y = y;
	}

	bool CheckCollision(const SDL_Rect& r) const;
};

class j1Collision : public j1Module
{
public:

	j1Collision();
	~j1Collision();

	bool PreUpdate();
	bool Update(float dt);
	bool PostUpdate(float dt);
	bool CleanUp();

	Collider* AddCollider(SDL_Rect rect, COLLIDER_TYPE type, j1Module* callback = nullptr);
	void DebugDraw();

	p2List<Collider*> colliders;
	bool debug = true;

	void QueryCollisions(Collider &to_check) const;

private:

	bool matrix[static_cast<int>(COLLIDER_TYPE::COLLIDER_MAX)][static_cast<int>(COLLIDER_TYPE::COLLIDER_MAX)];
};

#endif // __j1Collision_H__