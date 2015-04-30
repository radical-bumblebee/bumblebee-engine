#include "BumblebeeRoot.h"

// World tick, updates systems
void BumblebeeRoot::tick() {
	int start = 0;
	int end = 0;
	if (world->scene()) {
		for (auto system : _systems) {
			system->tick();
		}
	}
}

// Main game loop
void BumblebeeRoot::loop() {
	if (_renderer) {
		SDL_Event e;
		bool quit = false;
		int start_time = SDL_GetTicks();
		int now = 0;
		int before = 0;
		int frames = 0;

		// Game loop
		while (!quit) {
			now = SDL_GetTicks();
			world->dt = (float)(now - before) / 1000;
			before = now;

			while (SDL_PollEvent(&e) != 0) {
				if (e.type == SDL_QUIT) {
					quit = true;
					break;
				}
				else if (e.type == SDL_KEYDOWN || e.type == SDL_KEYUP || e.type == SDL_MOUSEBUTTONDOWN || e.type == SDL_MOUSEBUTTONUP) {
					if (_input_callback) {
						_input_callback(e);
					}
				}
				else if (e.type == SDL_WINDOWEVENT) {
					switch (e.window.event) {
					case SDL_WINDOWEVENT_RESIZED:
						_renderer->resize(e.window.data1, e.window.data2);
						break;
					}
				}
			}

			this->tick();
			world->fps = (frames / (float)(SDL_GetTicks() - start_time)) * 1000;
			//SDL_SetWindowTitle(_renderer->window, (WINDOW_TITLE +  std::to_string((int)std::ceil(world->fps))).c_str());
			frames++;
		}
	}
}

// Attaches a core system
void BumblebeeRoot::register_system(SystemType type) {
	ISystem::ptr system = nullptr;
	switch (type) {
	case BUMBLEBEE_RENDERER:
		system = std::make_shared<BumblebeeRenderer>();
		system->init(world);
		_systems.push_back(system);
		_renderer = std::dynamic_pointer_cast<BumblebeeRenderer>(_systems.at(_systems.size() - 1));
		break;
	case BUMBLEBEE_PHYSICS:
		system = std::make_shared<BumblebeeBullet>();
		system->init(world);
		_systems.push_back(system);
		_physics = std::dynamic_pointer_cast<BumblebeeBullet>(_systems.at(_systems.size() - 1));
		break;
	}

	// Ensure systems run in the desired order
	if (system) {
		std::sort(_systems.begin(), _systems.end(), [](ISystem::ptr lhs, ISystem::ptr rhs) -> bool {
			return lhs->priority > rhs->priority;
		});
	}
}

// Attaches a user defined system
void BumblebeeRoot::register_custom_system(ISystem::ptr system) {
	if (system) {
		system->type = BUMBLEBEE_CUSTOM;
		system->priority = 2;
		system->init(world);
		_systems.push_back(system);

		std::sort(_systems.begin(), _systems.end(), [](ISystem::ptr lhs, ISystem::ptr rhs) -> bool {
			return lhs->priority > rhs->priority;
		});
	}
}

// Remove a core system
void BumblebeeRoot::unregister_system(SystemType type) {
	switch (type) {
	case BUMBLEBEE_RENDERER:
		_systems.erase(std::find(_systems.begin(), _systems.end(), _renderer));
		break;
	case BUMBLEBEE_PHYSICS:
		_systems.erase(std::find(_systems.begin(), _systems.end(), _physics));
		break;
	}
}

void BumblebeeRoot::register_input_callback(input_callback_type input_callback) {
	_input_callback = input_callback;
}

void BumblebeeRoot::register_ai_callback(ai_callback_type ai_callback) {
	_ai_callback = ai_callback;
}

void BumblebeeRoot::register_collision_callback(BumblebeeBullet::collision_callback_type collision_callback) {
	_physics->collision_callback = collision_callback;
}

// Initializes framework
bool BumblebeeRoot::init() {
	world = std::make_shared<BumblebeeWorld>();

	if (!world->init()) {
		Logger::get()->log("Failed to initialize world");
		return false;
	}

	SDL_StartTextInput();

	return true;
}

// Framework cleanup
void BumblebeeRoot::destroy() {
	// Remove all systems cleanly
	for (auto i = _systems.size(); i > 0; i--) {
		_systems[i-1]->destroy();
		_systems[i-1].reset();
	}

	world->destroy();
	world.reset();
}