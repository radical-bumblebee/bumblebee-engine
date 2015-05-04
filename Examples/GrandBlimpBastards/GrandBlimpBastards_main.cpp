#include <BumblebeeRoot.h>

#include <iostream>

#include "BlimpFactory.h"
#include "SceneDirector.h"
#include "SceneInfo.h"
#include "AI.h"

// Memory leak detection
#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#endif

BumblebeeRoot::uptr root;
SceneInfo::ptr scene_info;
SceneDirector::ptr director;
AI::ptr ai;

void input_callback(SDL_Event e);

int main(int argc, char* argv[]) {
#ifdef _DEBUG
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	//	_CrtSetBreakAlloc(164);
#endif

	// Initialize framework
	root = std::make_unique<BumblebeeRoot>();
	if (!root->init()) {
		Logger::get()->log("Failed to initialize the root");
		return EXIT_FAILURE;
	}

	scene_info = std::make_shared<SceneInfo>();
	director = std::make_shared<SceneDirector>(scene_info);

	ai = std::make_shared<AI>();

	// Initialize systems
	root->register_system(BUMBLEBEE_RENDERER);
	root->register_system(BUMBLEBEE_PHYSICS);
	root->register_custom_system(director);
	root->register_custom_system(ai);
	root->register_input_callback(&input_callback);
	root->register_collision_callback(std::bind(&SceneDirector::collision_callback, director, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));

	auto world = root->world;

	// Load assets
	world->proxy->load_model("assets/models/sphere.ply");
	world->proxy->load_model("assets/models/capsule.ply");
	world->proxy->load_model("assets/models/cube.ply");
	world->proxy->load_model("assets/models/plane.ply");

	std::vector<PlyVertex> vertices;
	vertices.push_back(PlyVertex(0.0f, 0.0f, 0.0f));
	vertices.push_back(PlyVertex(0.0f, 0.0f, 0.0f));
	world->proxy->load_model("line", vertices);

	world->proxy->load_texture("assets/textures/marble.dds");
	world->proxy->load_texture("assets/textures/red.dds");
	world->proxy->load_texture("assets/textures/metal.dds");
	world->proxy->load_texture("assets/textures/smoke.dds");
	world->proxy->load_texture("assets/textures/cobblestone.dds");
	world->proxy->load_skybox("assets/textures/", "tropical_neg_x.dds", "tropical_pos_x.dds", "tropical_pos_y.dds", "tropical_neg_y.dds", "tropical_pos_z.dds", "tropical_neg_z.dds");

	world->proxy->load_font("assets/fonts/times.ttf");

	world->proxy->load_particle("assets/textures/smoke.dds", 1000);

	director->init_scene();

	// Game loop
	root->loop();

	// Exit cleanly
	world.reset();
	root->destroy();
	root.reset();
	
	return EXIT_SUCCESS;
}

void input_callback(SDL_Event e) {
	if (e.type == SDL_KEYUP) {
		switch (e.key.keysym.sym) {
		case SDLK_o:
			scene_info->rotate = !scene_info->rotate;
			break;
		case SDLK_r:
			// Reset scene
			if (root->world->scene()) {
				auto old_scene_id = scene_info->scene_id;
				director->init_scene();
				director->high_score = 0;
				director->difficulty = 0.0f;
				ai->difficulty_setting = 0.0f;
				ai->spawn_setting = 6.0f;
				ai->spawn_timer = 1.0f;
				ai->fire_timer = 1.5f;
				root->world->config->game_over = 1.0f;
				root->world->scene()->objects->at(scene_info->skybox_id)->model->info->transparency = 1.0f;
				
				root->world->remove_scene(old_scene_id);
			}
			break;
		}
	}
	else if (e.type == SDL_MOUSEBUTTONUP) {
		switch (e.button.button) {
		case SDL_BUTTON_LEFT:
			director->fire_cannon();
			break;
		}
	}
}