#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Input.h"
#include "j1Textures.h"
#include "j1Audio.h"
#include "j1Render.h"
#include "j1Window.h"
#include "j1Map.h"
#include "j1Scene.h"

#include "j1Collision.h"
#include "j1App.h"

j1Scene::j1Scene() : j1Module()
{
	name.create("scene");
}

// Destructor
j1Scene::~j1Scene()
{}

// Called before render is available
bool j1Scene::Awake(pugi::xml_node& config)
{
	LOG("Loading Scene");
	bool ret = true;

	map_name = config.child("currentmap").attribute("name").as_string();

	if (map_name == NULL)
	{
		ret = false;
	}
	
	return ret;
}

// Called before the first frame
bool j1Scene::Start()
{
	bool ret = true;

	//Loading map
	ret = App->map->Load(map_name.GetString());

	p2SString sadpiano("%s%s", App->audio->musicfolder.GetString(), "stage1.ogg");

	//Loading music sample
	App->audio->PlayMusic(sadpiano.GetString());

	if (colliderfloor == nullptr)
		colliderfloor = App->coll->AddCollider({ 0, 150, 1024, 100 }, COLLIDER_FLOOR, this);
	else
		colliderfloor->SetPos(0, 0);

	if (colliderbox == nullptr)
		colliderbox = App->coll->AddCollider({ 100, 120, 50, 30 }, COLLIDER_FLOOR, this);
	else
		colliderbox->SetPos(0, 0);

	if (!ret)
	{
		ret = false;
	}
	
	return ret;
}

// Called each loop iteration
bool j1Scene::PreUpdate()
{
	return true;
}

// Called each loop iteration
bool j1Scene::Update(float dt)
{
	if (App->input->GetKey(SDL_SCANCODE_U) == KEY_DOWN)
	{
		App->audio->ChangeVolume_music(10);
		App->audio->ChangeVolume_fx(10);
		LOG("volume up");
	}

	if (App->input->GetKey(SDL_SCANCODE_T) == KEY_DOWN)
	{
		App->audio->ChangeVolume_music(-10);
		App->audio->ChangeVolume_fx(-10);
		LOG("volume down");
	}


	if(App->input->GetKey(SDL_SCANCODE_L) == KEY_DOWN)
		App->LoadGame("save_game.xml");

	if(App->input->GetKey(SDL_SCANCODE_K) == KEY_DOWN)
		App->SaveGame("save_game.xml");

	if(App->input->GetKey(SDL_SCANCODE_UP) == KEY_REPEAT)
		App->render->camera.y += 1;

	if(App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT)
		App->render->camera.y -= 1;

	if(App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT)
		App->render->camera.x += 1;

	if(App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT)
		App->render->camera.x -= 1;

	App->map->Draw();

	int x, y;
	App->input->GetMousePosition(x, y);
	iPoint map_coordinates = App->map->WorldToMap(x - App->render->camera.x, y - App->render->camera.y);
	p2SString title("Map:%dx%d Tiles:%dx%d Tilesets:%d Tile:%d,%d",
					App->map->data.width, App->map->data.height,
					App->map->data.tile_width, App->map->data.tile_height,
					App->map->data.tilesets.count(),
					map_coordinates.x, map_coordinates.y);

	App->win->SetTitle(title.GetString());

	App->render->DrawQuad(colliderfloor->rect, 0, 0, 255);
	App->render->DrawQuad(colliderbox->rect, 0, 255, 0);
	return true;
}

// Called each loop iteration
bool j1Scene::PostUpdate()
{
	bool ret = true;

	if(App->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)
		ret = false;

	return ret;
}

// Called before quitting
bool j1Scene::CleanUp()
{
	LOG("Freeing scene");

	if(colliderbox!=nullptr)
	colliderbox = nullptr;

	if(colliderfloor!=nullptr)
	colliderfloor = nullptr;

	return true;
}
