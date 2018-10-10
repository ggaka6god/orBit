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

//#include "j1Collision.h"

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

	for (pugi::xml_node stage = config.child("map_name"); stage; stage = stage.next_sibling("map_name"))
	{
		p2SString* StageName = new p2SString;

		StageName->create(stage.attribute("name").as_string());
		StageList.add(StageName);
	}

	map_name = config.child("map_name").attribute("name").as_string();

	if (StageList.start->data->GetString() == NULL)
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
	ret = App->map->Load(StageList.start->data->GetString());
	FirstStage = StageList.start->data->GetString();


	if (FirstStage == "stage1_TiledV017.tmx")
	{

		p2SString stageMusic("%s%s", App->audio->musicfolder.GetString(), App->audio->SongNamesList.start->data->GetString());
		App->audio->PlayMusic(stageMusic.GetString());
	}
	else
	{
		p2SString stageMusic("%s%s", App->audio->musicfolder.GetString(), App->audio->SongNamesList.start->next->data->GetString());
		App->audio->PlayMusic(stageMusic.GetString());
	}

	//if (colliderfloor == nullptr)
	//	colliderfloor = App->coll->AddCollider({ 0, 150, 1024, 100 }, COLLIDER_FLOOR, this);
	///*else
	//	colliderfloor->SetPos(0, 0);*/

	//if (colliderbox == nullptr)
	//	colliderbox = App->coll->AddCollider({ 100, 120, 50, 30 }, COLLIDER_FLOOR, this);
	////else
	////	colliderbox->SetPos(0, 0);

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
	if (App->input->GetKey(SDL_SCANCODE_1) == KEY_DOWN && firstStage == false) //can only press during second stage. gos to first stage
	{	
			change_scene(StageList.start->data->GetString());
			firstStage = true;
			secondStage = false;
		}

	if (App->input->GetKey(SDL_SCANCODE_2) == KEY_DOWN && secondStage== false) //can only press during first stage. gos to second stage
	{	
		change_scene(StageList.start->next->data->GetString());
		firstStage = false;
		secondStage = true;
	}
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

	if(App->input->GetKey(SDL_SCANCODE_S) == KEY_DOWN)
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

	/*App->render->DrawQuad(colliderfloor->rect, 0, 0, 255);
	App->render->DrawQuad(colliderbox->rect, 0, 255, 0);*/
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

	/*if (colliderbox != nullptr)
		colliderbox = nullptr;

	if (colliderfloor != nullptr)
		colliderfloor = nullptr;*/

	p2List_item<p2SString*>* item;
	item = StageList.start;

	while (item != NULL)
	{
		RELEASE(item->data);
		item = item->next;
	}
	StageList.clear();

	return true;
}


bool j1Scene::change_scene(const char* map_name) {
	bool ret = true;
	App->map->CleanUp();
	App->map->Load(map_name);
	if (FirstStage == map_name)
	{
		p2SString stageMusic("%s%s", App->audio->musicfolder.GetString(), App->audio->SongNamesList.start->data->GetString());//aqui deberia poder leer metadata
		App->audio->PlayMusic(stageMusic.GetString());
	}
	else{
		p2SString stageMusic("%s%s", App->audio->musicfolder.GetString(), App->audio->SongNamesList.start->next->data->GetString());//aqui leer metadata de direccion
		App->audio->PlayMusic(stageMusic.GetString());
	}
	
	

	return ret;
}

