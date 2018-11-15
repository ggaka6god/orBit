#ifndef __J1ENTITY_H__
#define __J1ENTITY_H__

#include "p2SString.h"
#include "p2Point.h"

class j1EntityManager;
class Collider;



class j1Entity
{
public:

	j1Entity() : name("Unnamed"), manager(NULL)
	{
		position.SetToZero();
	}

	void Init(j1EntityManager* manager)
	{
		this->manager = manager;
	}

	// Called before the first frame if it was activated before that
	virtual void Start()
	{}

	// Called each loop iteration
	virtual void FixedUpdate(float dt)
	{}

	// Called each logic iteration
	virtual void LogicUpdate(float dt)
	{}

	// Called before quitting
	virtual void CleanUp()
	{}

	virtual void OnCollision(Collider* c1, Collider* c2)
	{}

	
public:

	p2SString			name;
	fPoint			position;
	Collider*     entitycoll;


private:

	j1EntityManager*	manager;
};

#endif // __J1ENTITY_H__
