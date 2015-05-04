#include "BumblebeeRoot.h"

// World tick, updates systems
void BumblebeeRoot::tick() {
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
			// Measure delta time
			now = SDL_GetTicks();
			world->dt = (float)(now - before) / 1000;
			before = now;

			while (SDL_PollEvent(&e) != 0) {
				if (e.type == SDL_QUIT) {
					// Exit application
					quit = true;
					break;
				}
				else if (e.type == SDL_KEYDOWN || e.type == SDL_KEYUP || e.type == SDL_MOUSEBUTTONDOWN || e.type == SDL_MOUSEBUTTONUP) {
					// Pass input event out of framework
					if (_input_callback) {
						_input_callback(e);
					}
				}
				else if (e.type == SDL_WINDOWEVENT) {
					switch (e.window.event) {
					case SDL_WINDOWEVENT_RESIZED:
						// Pass resize event to renderer
						_renderer->resize(e.window.data1, e.window.data2);
						break;
					}
				}
			}

			this->tick();

			// Calculate fps (badly)
			world->fps = (frames / (float)(SDL_GetTicks() - start_time)) * 1000;
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

// Dispatch input events outside framework
void BumblebeeRoot::register_input_callback(input_callback_type input_callback) {
	_input_callback = input_callback;
}

// Dispatch collision events outside framework
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
	// Shutdown all systems
	for (auto i = _systems.size(); i > 0; i--) {
		_systems[i-1]->destroy();
		_systems[i-1].reset();
	}

	world->destroy();
	world.reset();
}