#include <BumblebeeRoot.h>

#include "SceneInfo.h"
#include "DemoFactory.h"
#include "SceneDirector_old.h"
#include "BumblebeeInput.h"
#include "BumblebeePersist.h"

#include "ExampleHook.h"

#define PLAYER_ID 6

int ex::example_hook() {

	BumblebeeRoot::uptr root = std::make_unique<BumblebeeRoot>();

	// Initialize framework
	if (!root->init()) {
		Logger::get()->log("Failed to initialize the root");
		return EXIT_FAILURE;
	}

	SceneInfo::ptr scene_info = std::make_unique<SceneInfo>();
	DemoFactory::ptr factory = std::make_shared<DemoFactory>(root->world);

	SceneDirector::ptr director = std::make_unique<SceneDirector>(scene_info, factory);
	BumblebeeInput::ptr input_system = std::make_shared<BumblebeeInput>(scene_info);

	root->register_system(BUMBLEBEE_RENDERER);
	root->register_system(BUMBLEBEE_PHYSICS);

	root->register_custom_system(director);
	root->register_custom_system(input_system);

	auto world = root->world;

	// Load models
	world->proxy->load_model("assets/models/sphere.ply");
	world->proxy->load_model("assets/models/cube.ply");
	world->proxy->load_model("assets/models/cylinder.ply");
	world->proxy->load_model("assets/models/plane.ply");

	world->proxy->load_texture("assets/textures/brick.dds");
	world->proxy->load_texture("assets/textures/marble.dds");
	world->proxy->load_texture("assets/textures/cobblestone.dds");

	world->proxy->load_font("assets/fonts/times.ttf");

	// Scene 1
	auto scene_id = world->create_scene();
	world->activate_scene(scene_id);
	world->scene()->camera()->eye = glm::vec3(10.0f, 10.0f, 0.0f);
	world->scene()->camera()->target = glm::vec3(0.0f, 5.0f, 0.0f);
	
	// Create objects
	world->scene()->player_id = world->scene()->objects->add(factory->create_object(0.0f, 5.0f, 0.0f));
	director->generate_segment(2, false);	

	// Create lights
	world->scene()->lights->add(factory->create_light(POINT_LIGHT, 0.0f, 2.0f, 0.0f));
	world->scene()->lights->add(factory->create_light(POINT_LIGHT, 0.0f, 8.0f, 0.0f));

	std::string high_score_string = "High Score: -";
	world->scene()->ui->add(std::make_shared<BumblebeeUIElement>("Current Score: 0", "assets/fonts/times.ttf", 10.0f, 30.0f, &glm::vec4(1.0f)));
	auto high_score_ui = world->scene()->ui->add(std::make_shared<BumblebeeUIElement>(high_score_string, "assets/fonts/times.ttf", 10.0f, 5.0f, &glm::vec4(1.0f)));
	int high_score = BumblebeePersist::get()->get_high_score(PLAYER_ID);
	if (high_score > 0) {
		high_score_string = "High Score: " + std::to_string(high_score);
		world->scene()->ui->at(high_score_ui)->text = high_score_string;
	}

	// Scene 2
	world->activate_scene(world->create_scene());
	world->scene()->player_id = world->scene()->objects->add(factory->create_object(0.0f, 2.0f, 0.0f));

	world->scene()->objects->add(factory->create_plane(0.0f, 0.0f, 0.0f));
	
	world->scene()->objects->add(factory->create_cylinder(-PILLAR_LENGTH, 1.0f, PILLAR_LENGTH));
	world->scene()->objects->add(factory->create_cylinder(PILLAR_LENGTH, 1.0f, PILLAR_LENGTH));
	world->scene()->objects->add(factory->create_cylinder(-PILLAR_LENGTH, 1.0f, -PILLAR_LENGTH));
	world->scene()->objects->add(factory->create_cylinder(PILLAR_LENGTH, 1.0f, -PILLAR_LENGTH));
	
	world->scene()->lights->add(factory->create_light(POINT_LIGHT, 0.0f, 2.0f, 0.0f));
	world->scene()->lights->add(factory->create_light(POINT_LIGHT, 0.0f, 8.0f, 0.0f));
	world->scene()->lights->add(factory->create_light(DIRECTIONAL_LIGHT, 0.0f, 10.0f, 0.0f));

	world->activate_scene(scene_id);
	root->input_callback = std::bind(&BumblebeeInput::key_click, input_system, std::placeholders::_1);
	root->mouse_callback = std::bind(&BumblebeeInput::mouse_event, input_system, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);

	// Game loop
	root->loop();

	// Uploads high score
	if (director->score > (unsigned int)high_score) {
		BumblebeePersist::get()->set_high_score(PLAYER_ID, director->score);
	}

	// Exit cleanly
	world.reset();
	root->destroy();
	root.reset();

	return EXIT_SUCCESS;
}