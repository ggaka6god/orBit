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
#include "j1Player.h"
#include "j1PathFinding.h"
#include "j1EntityManager.h"
#include "j1Slime.h"
#include "j1Bat.h"
#include "Brofiler\Brofiler.h"


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
		p2SString* StageName = new p2SString(stage.attribute("name").as_string());
		StageList.add(StageName);
			App->map->numberStages++;
	}


	if (StageList.start->data->GetString() == NULL)
	{
		ret = false;
		LOG("stagelist is null");
	}
	camera1.x = config.child("startcamera1").attribute("x").as_int();
	camera1.y = config.child("startcamera1").attribute("y").as_int();
	camera2.x = config.child("startcamera2").attribute("x").as_int();
	camera2.y = config.child("startcamera2").attribute("y").as_int();

	areaofcollision = config.child("areaofcollision").attribute("value").as_int();

	return ret;
}

// Called before the first frame
bool j1Scene::Start()
{
	bool ret = true;


	// --- Creating entity  ---
	player = (j1Player*)App->entities->CreateEntity("player", entity_type::PLAYER);
	
	bat = (j1Bat*)App->entities->CreateEntity("bat", entity_type::BAT);
	bat2 = (j1Bat*)App->entities->CreateEntity("bat", entity_type::BAT);

	slime = (j1Slime*)App->entities->CreateEntity("slime", entity_type::SLIME);
	slime2 = (j1Slime*)App->entities->CreateEntity("slime", entity_type::SLIME);

	//Loading both maps

	p2List_item<p2SString*>* stageListItem;
	stageListItem = StageList.start;

	ret = App->map->Load(stageListItem->data->GetString(),App->map->data);
		
		if (ret == false)
		{
			LOG("issue loading First Stage");
			ret = false;
		}
		
	stageListItem = StageList.start->next;;

	ret = App->map->Load(stageListItem->data->GetString(), App->map->data2);

	if (ret == false)
	{
		LOG("issue loading Second Stage");
		ret = false;
	}


	//loading music & positions depending on the order

	FirstStage = StageList.start->data->GetString();

	if (FirstStage =="stage1_TiledV017.tmx" )
	{
		App->render->camera.x = camera1.x;
		App->render->camera.y = camera1.y;

		player->position.x = App->map->data.initpos.x;
		player->position.y = App->map->data.initpos.y;
		
		slime->position.x = App->map->data.slime1.x;
		slime->position.y = App->map->data.slime1.y;

		slime2->position.x = App->map->data.slime2.x;
		slime2->position.y = App->map->data.slime2.y;

		bat->position.x = App->map->data.bat1.x;
		bat->position.y = App->map->data.bat1.y;

		bat2->position.x = App->map->data.bat2.x;
		bat2->position.y = App->map->data.bat2.y;

		p2SString stageMusic("%s%s", App->audio->musicfolder.GetString(), App->audio->SongNamesList.start->data->GetString());
		App->audio->PlayMusic(stageMusic.GetString());

		// --- Pathfinding walkability map 1 ---

		int w, h;
		uchar* buffer_data = NULL;
		if (App->map->CreateWalkabilityMap(w, h, &buffer_data,App->map->data))
			App->pathfinding->SetMap(w, h, buffer_data);

		RELEASE_ARRAY(buffer_data);
	}
	else
	{
		App->render->camera.x = camera2.x;
		App->render->camera.y = camera2.y;

		player->position.x = App->map->data2.initpos.x;
		player->position.y = App->map->data2.initpos.y;

		slime->position.x = App->map->data2.slime1.x;
		slime->position.y = App->map->data2.slime1.y;

		slime2->position.x = App->map->data2.slime2.x;
		slime2->position.y = App->map->data2.slime2.y;

		bat->position.x = App->map->data2.bat1.x;
		bat->position.y = App->map->data2.bat1.y;

		bat2->position.x = App->map->data2.bat2.x;
		bat2->position.y = App->map->data2.bat2.y;

		p2SString stageMusic("%s%s", App->audio->musicfolder.GetString(), App->audio->SongNamesList.start->next->data->GetString());
		App->audio->PlayMusic(stageMusic.GetString());


		// --- Pathfinding walkability map 2 ---

		int w, h;
		uchar* buffer_data = NULL;
		if (App->map->CreateWalkabilityMap(w, h, &buffer_data, App->map->data2))
			App->pathfinding->SetMap(w, h, buffer_data);

		RELEASE_ARRAY(buffer_data);
	}

	// --- Initial position for enemies ---
	xSlime = slime->position.x;
	ySlime = slime->position.y;

	xSlime2 = slime2->position.x;
	ySlime2 = slime2->position.y;

	xBat = bat->position.x;
	yBat = bat->position.y;

	xBat2 = bat2->position.x;
	yBat2 = bat2->position.y;


	App->map->ColliderDrawer(App->map->data);

	return ret;
}

// Called each loop iteration
bool j1Scene::PreUpdate()
{

	BROFILER_CATEGORY("Scene_Pre__Update", Profiler::Color::MediumSeaGreen);


	// debug pathfing ------------------
	static iPoint origin;
	static bool origin_selected = false;

	int x, y;
	App->input->GetMousePosition(x, y);
	iPoint p = App->render->ScreenToWorld(x, y);
	if(firstStage)
	p = App->map->WorldToMap(p.x, p.y, App->map->data);
	else
	p = App->map->WorldToMap(p.x, p.y, App->map->data2);

	if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_DOWN)
	{
		if (origin_selected == true)
		{
			App->pathfinding->CreatePath(origin, p);
			origin_selected = false;
		}
		else
		{
			origin = p;
			origin_selected = true;
		}
	}

	//win condition
	if (firstStage && (player->position.x >= App->map->data.finalpos.x) && (player->position.y <= App->map->data.finalpos.y))
	{
		change_scene(StageList.start->next->data->GetString());
		firstStage = false;
		secondStage = true;
	}

	else if (secondStage && (player->position.x >= App->map->data2.finalpos.x) && (player->position.y <= App->map->data2.finalpos.y))
	{
		change_scene(StageList.start->data->GetString());
		firstStage = true;
		secondStage = false;
	}
	return true;
}

// Called each loop iteration
bool j1Scene::Update(float dt)
{
	BROFILER_CATEGORY("Scene_Update", Profiler::Color::MediumSpringGreen);

	if (App->input->GetKey(SDL_SCANCODE_1) == KEY_DOWN && firstStage == false) //can only press during second stage. goes to first stage
	{	
			change_scene(StageList.start->data->GetString());
			firstStage = true;
			secondStage = false;
	}

	if (App->input->GetKey(SDL_SCANCODE_2) == KEY_DOWN && secondStage== false) //can only press during first stage. goes to second stage
	{	
		change_scene(StageList.start->next->data->GetString());
		firstStage = false;
		secondStage = true;
	}

	if (App->input->GetKey(SDL_SCANCODE_3) == KEY_DOWN ) //test sounds
	{
		App->audio->PlayFx(App->audio->doublejumpfx, 0);
	}

	if (App->input->GetKey(SDL_SCANCODE_F1) == KEY_DOWN) //reload stage1
	{
		
		change_scene(StageList.start->data->GetString());
		firstStage = true;
		secondStage = false;
	}

	if (App->input->GetKey(SDL_SCANCODE_F2) == KEY_DOWN) // beginning of current level
	{
		if (firstStage)
			{
				change_scene(StageList.start->data->GetString());
				firstStage = true;
				secondStage = false;
			}
		else if (secondStage)
			{
				change_scene(StageList.start->next->data->GetString());
				firstStage = false;
				secondStage = true;
			}
	}

	if (App->input->GetKey(SDL_SCANCODE_KP_PLUS) == KEY_DOWN) 
	{
		App->audio->ChangeVolume_music(10);
		App->audio->ChangeVolume_fx(10);
		LOG("volume up");
	}

	if (App->input->GetKey(SDL_SCANCODE_KP_MINUS) == KEY_DOWN)
	{
		App->audio->ChangeVolume_music(-10);
		App->audio->ChangeVolume_fx(-10);
		LOG("volume down");
	}

	if (App->input->GetKey(SDL_SCANCODE_F6) == KEY_DOWN)
	{	

		bool ret = App->LoadGame("save_game.xml");
	}

	if(App->input->GetKey(SDL_SCANCODE_F5) == KEY_DOWN) 
		App->SaveGame("save_game.xml");

	if (App->input->GetKey(SDL_SCANCODE_F11) == KEY_DOWN)
		App->cap_on = !App->cap_on;

	if(App->input->GetKey(SDL_SCANCODE_UP) == KEY_REPEAT)
		App->render->camera.y += 2;

	if(App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT)
		App->render->camera.y -= 2;

	if (App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT)
		App->render->camera.x -= 2;

	if (App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT)
		App->render->camera.x += 2;

	int x, y;
	App->input->GetMousePosition(x, y);


	if (firstStage == true)
	{
		App->map->Draw(App->map->data);


		iPoint map_coordinates = App->map->WorldToMap(x - App->render->camera.x, y - App->render->camera.y, App->map->data);
		p2SString title("Map:%dx%d Tiles:%dx%d Tilesets:%d Tile:%d,%d",
			App->map->data.width, App->map->data.height,
			App->map->data.tile_width, App->map->data.tile_height,
			App->map->data.tilesets.count(),
			map_coordinates.x, map_coordinates.y);
		//Debug purpose
		//App->win->SetTitle(title.GetString());
	}
	else
	{
		App->map->Draw(App->map->data2);


		iPoint map_coordinates = App->map->WorldToMap(x - App->render->camera.x, y - App->render->camera.y, App->map->data2);
		p2SString title("Map:%dx%d Tiles:%dx%d Tilesets:%d Tile:%d,%d",
			App->map->data.width, App->map->data.height,
			App->map->data.tile_width, App->map->data.tile_height,
			App->map->data.tilesets.count(),
			map_coordinates.x, map_coordinates.y);
		//Debug purpose
		//App->win->SetTitle(title.GetString());
	}

	// --- Debug Pathfinding
	if (App->coll->debug)
	{
		iPoint p = App->render->ScreenToWorld(x, y);
		if (firstStage)
		{
			p = App->map->WorldToMap(p.x, p.y, App->map->data);
			p = App->map->MapToWorld(p.x, p.y, App->map->data);
			App->render->Blit(App->map->data.tilesets.start->next->next->data->texture, p.x, p.y, &debug_Tex_rect);
		}
		else
		{
			p = App->map->WorldToMap(p.x, p.y, App->map->data2);
			p = App->map->MapToWorld(p.x, p.y, App->map->data2);
			App->render->Blit(App->map->data2.tilesets.start->next->next->data->texture, p.x, p.y, &debug_Tex_rect);
		}

		const p2DynArray<iPoint>* path = App->pathfinding->GetLastPath();

		for (uint i = 0; i < path->Count(); ++i)
		{
			if (firstStage)
			{
				iPoint pos = App->map->MapToWorld(path->At(i)->x, path->At(i)->y, App->map->data);
				App->render->Blit(App->map->data.tilesets.start->next->next->data->texture, pos.x, pos.y, &debug_Tex_rect);
			}
			else
			{
				iPoint pos = App->map->MapToWorld(path->At(i)->x, path->At(i)->y, App->map->data2);
				App->render->Blit(App->map->data2.tilesets.start->next->next->data->texture, pos.x, pos.y, &debug_Tex_rect);
			}


		}
	}



	return true;
}

// Called each loop iteration
bool j1Scene::PostUpdate(float dt)
{
	BROFILER_CATEGORY("Scene_Post_Update", Profiler::Color::MediumTurquoise);

	bool ret = true;

	if(App->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)
		ret = false;

	// --- Controlling camera ---

	// --- Camera In X ---
	App->render->camera.x = (-player->Future_position.x*App->win->GetScale() - player->entitycoll->rect.w / 2 + App->render->camera.w / 2);

	// --- Keeping camera on axis X bounds ---
	if (-App->render->camera.x <= 2)
	{
		App->render->camera.x = -2;
	}

	if (-App->render->camera.x + App->render->camera.w >= App->map->data.width*App->map->data.tile_width*App->win->GetScale())
	{
		App->render->camera.x = -App->map->data.width*App->map->data.tile_width*App->win->GetScale() + App->render->camera.w;
	}

	// --- Camera In Y ---

	// --- Camera down ---

	if (player->Future_position.y*App->win->GetScale() + player->entitycoll->rect.h >= -App->render->camera.y + App->render->camera.h - App->render->camera.h / 6)
	{
		App->render->camera.y = -(player->Future_position.y* App->win->GetScale() + player->entitycoll->rect.h - App->render->camera.h +App->render->camera.h / 6);
	}

	//// --- Camera up ---

	if (player->Future_position.y*App->win->GetScale() <= -App->render->camera.y + App->render->camera.h / 6)
	{
		App->render->camera.y = -(player->Future_position.y* App->win->GetScale() - App->render->camera.h / 6);
	}

	// --- Keeping camera on axis Y bounds ---

	if (-App->render->camera.y + App->render->camera.h > App->map->data.height*App->map->data.tile_height*App->win->GetScale())
	{
		App->render->camera.y = (-App->map->data.height*App->map->data.tile_height*App->win->GetScale() + App->render->camera.h);
	}

	if (App->render->camera.y > 0.0)
	{
		App->render->camera.y = 0.0f;
	}

	//-------------------

	return ret;
}

// Called before quitting
bool j1Scene::CleanUp()
{
	LOG("Freeing scene");

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

	App->map->paralaxRef[0] = App->map->offset;
	App->map->paralaxRef[1] = App->map->offset;

	player->parallaxflow = 0;
	player->previousflow = 0;

	App->coll->CleanUp();

	App->entities->DestroyEntity(bat);
	App->entities->DestroyEntity(bat2);
	App->entities->DestroyEntity(slime);
	App->entities->DestroyEntity(slime2);

	player->entitycoll= App->coll->AddCollider(player->entitycollrect,COLLIDER_TYPE::COLLIDER_PLAYER, App->entities);

	bat = (j1Bat*)App->entities->CreateEntity("bat", entity_type::BAT);
	bat2 = (j1Bat*)App->entities->CreateEntity("bat", entity_type::BAT);

	slime = (j1Slime*)App->entities->CreateEntity("slime", entity_type::SLIME);
	slime2 = (j1Slime*)App->entities->CreateEntity("slime", entity_type::SLIME);


	if (FirstStage == map_name)
	{	
		App->render->camera.x = camera1.x;
		App->render->camera.y = camera1.y;

		player->position.x = App->map->data.initpos.x;
		player->position.y = App->map->data.initpos.y;

		slime->position.x = App->map->data.slime1.x;
		slime->position.y = App->map->data.slime1.y;

		slime2->position.x = App->map->data.slime2.x;
		slime2->position.y = App->map->data.slime2.y;

		bat->position.x = App->map->data.bat1.x;
		bat->position.y = App->map->data.bat1.y;

		bat2->position.x = App->map->data.bat2.x;
		bat2->position.y = App->map->data.bat2.y;

		App->map->ColliderDrawer(App->map->data);
		p2SString stageMusic("%s%s", App->audio->musicfolder.GetString(), App->audio->SongNamesList.start->data->GetString());
		App->audio->PlayMusic(stageMusic.GetString());
		player->entitystate = FALLING;


		// --- Pathfinding walkability map 1 ---

		int w, h;
		uchar* buffer_data = NULL;
		if (App->map->CreateWalkabilityMap(w, h, &buffer_data, App->map->data))
			App->pathfinding->SetMap(w, h, buffer_data);

		RELEASE_ARRAY(buffer_data);
	}
	else
	{
		App->render->camera.x = camera2.x;
		App->render->camera.y = camera2.y;

		player->position.x = App->map->data2.initpos.x;
		player->position.y = App->map->data2.initpos.y;

		slime->position.x = App->map->data2.slime1.x;
		slime->position.y = App->map->data2.slime1.y;

		slime2->position.x = App->map->data2.slime2.x;
		slime2->position.y = App->map->data2.slime2.y;

		bat->position.x = App->map->data2.bat1.x;
		bat->position.y = App->map->data2.bat1.y;

		bat2->position.x = App->map->data2.bat2.x;
		bat2->position.y = App->map->data2.bat2.y;


		App->map->ColliderDrawer(App->map->data2);
		p2SString stageMusic("%s%s", App->audio->musicfolder.GetString(), App->audio->SongNamesList.start->next->data->GetString());
		App->audio->PlayMusic(stageMusic.GetString());
		player->entitystate = FALLING;


		// --- Pathfinding walkability map 2 ---

		int w, h;
		uchar* buffer_data = NULL;
		if (App->map->CreateWalkabilityMap(w, h, &buffer_data, App->map->data2))
			App->pathfinding->SetMap(w, h, buffer_data);

		RELEASE_ARRAY(buffer_data);
	}
	slime->entitycoll = App->coll->AddCollider(slime->entitycollrect, COLLIDER_TYPE::COLLIDER_ENEMY_SLIME, App->entities);
	slime->entitycoll->SetPos(slime->position.x, slime->position.y);

	slime2->entitycoll = App->coll->AddCollider(slime2->entitycollrect, COLLIDER_TYPE::COLLIDER_ENEMY_SLIME, App->entities);
	slime2->entitycoll->SetPos(slime2->position.x, slime2->position.y);

	bat->entitycoll = App->coll->AddCollider(bat->entitycollrect, COLLIDER_TYPE::COLLIDER_ENEMY_BAT, App->entities);
	bat->entitycoll->SetPos(bat->position.x, bat->position.y);

	bat2->entitycoll = App->coll->AddCollider(bat2->entitycollrect, COLLIDER_TYPE::COLLIDER_ENEMY_BAT, App->entities);
	bat2->entitycoll->SetPos(bat2->position.x, bat2->position.y);

	return ret;
}


bool j1Scene::Save(pugi::xml_node &config) const
{
	bool ret = true;

	config.append_child("firstStage").append_attribute("value") = firstStage;
	config.append_child("secondStage").append_attribute("value") = secondStage;

	 xSlime = slime->position.x;
	 ySlime = slime->position.y;

	 xSlime2 = slime2->position.x;
	 ySlime2 = slime2->position.y;

	 xBat = bat->position.x;
	 yBat = bat->position.y;

	 xBat2 = bat2->position.x;
	 yBat2 = bat2->position.y;

	return ret;
}

bool j1Scene::Load(pugi::xml_node &config)
{

	bool ret = true;
	int x = player->position.x;
	int y = player->position.y;

	

	DestinationStage1 = config.child("firstStage").attribute("value").as_bool();
	DestinationStage2 = config.child("secondStage").attribute("value").as_bool();

	if (firstStage)
	{

		//stage 2
		if (DestinationStage2)
		{
			change_scene(StageList.start->next->data->GetString());
			secondStage = true;
			firstStage = false;
			
		}

		else //stage 1
		{
			change_scene(StageList.start->data->GetString());
			firstStage = true;
			secondStage = false;

		}

	}

	else if (secondStage)
	{
		//stage1
		if (DestinationStage1)
		{
			change_scene(StageList.start->data->GetString());
			firstStage = true;
			secondStage = false;
			
		}
		//stage2
		else
		{
			change_scene(StageList.start->next->data->GetString());
			firstStage = false;
			secondStage = true;
			
		}
	}
	player->position.x = x;
	player->position.y = y;
	

	slime->position.x = xSlime;
	slime->position.y = ySlime;

	slime2->position.x = xSlime2;
	slime2->position.y = ySlime2;

	bat->position.x = xBat;
	bat->position.y = yBat;

	bat2->position.x = xBat2;
	bat2->position.y = yBat2;

	slime->entitycoll->SetPos(slime->position.x, slime->position.y);
	
	slime2->entitycoll->SetPos(slime2->position.x, slime2->position.y);

	bat->entitycoll->SetPos(bat->position.x, bat->position.y);

	bat2->entitycoll->SetPos(bat2->position.x, bat2->position.y);

	return ret;
}