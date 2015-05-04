#ifndef _BumblebeeInput_H_
#define _BumblebeeInput_H_

#include <memory>
#include <vector>
#include <Dependencies/include/SDL/SDL.h>

#include "SceneInfo.h"
#include <ISystem.h>
#include <BumblebeePersist.h>

// Keycodes https://wiki.libsdl.org/SDLKeycodeLookup
class BumblebeeInput : public ISystem {
public:
	typedef std::shared_ptr<BumblebeeInput> ptr;

	//BumblebeeInput() { };
	BumblebeeInput(SceneInfo::ptr scene_info) : _scene_info(scene_info), _chump_flag(false) { };
	virtual ~BumblebeeInput() { };

	virtual bool init(BumblebeeWorld::ptr world);
	virtual void destroy();
	virtual void tick();

	void key_click(SDL_KeyboardEvent e);
	void mouse_event(SDL_Event e, int winx, int winy);

	class BeeMouse {
	public:
		BeeMouse() { };

		int x;
		int y;
	};

	// These are the keys that a bumblebee needs!
	class BeeKeys {
	public:
		typedef std::shared_ptr<BeeKeys> ptr;

		BeeKeys();
		~BeeKeys() { 
			if (keys)
				delete keys;
		};

		bool pressed(unsigned char key);
		void set_pressed(unsigned char key, bool value);

		bool* keys;

		bool up;
		bool down;
		bool left;
		bool right;
	};	

	int win_x;
	int win_y;

	BeeKeys::ptr tapped_keys;
	BeeKeys::ptr held_keys;
	BeeMouse mouse;
	void set_mouse_xy(int x, int y);
	void set_key(unsigned char key, bool pressed_value);
	void set_special_key(int key, bool pressed_value);

private:
	BumblebeeWorld::ptr _world;
	SceneInfo::ptr _scene_info;

	bool _chump_flag;

};

#endif