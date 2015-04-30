#ifndef _BumblebeeProxy_H_
#define _BumblebeeProxy_H_

#include "ModelInfo.h"
#include "ModelLoader.h"
#include "BumblebeeFontAtlas.h"
#include "ParticleInfo.h"

#include <memory>
#include <unordered_map>
#include <functional>

class BumblebeeProxy {
public:
	typedef std::shared_ptr<BumblebeeProxy> ptr;
	typedef std::function<void(ModelInfo::ptr)> model_callback_type;
	typedef std::function<unsigned int(const char*)> texture_callback_type;
	typedef std::function<FT_Face(const char*)> font_callback_type;
	typedef std::function<unsigned int(const char*, const char*, const char*, const char*, const char*, const char*)> skybox_callback_type;
	typedef std::function<void(ParticleInfo::ptr)> particle_callback_type;

	BumblebeeProxy() { };
	~BumblebeeProxy() { };

	bool init();
	void destroy();

	model_callback_type model_added_callback;
	texture_callback_type texture_added_callback;
	font_callback_type font_added_callback;
	skybox_callback_type skybox_added_callback;
	particle_callback_type particle_added_callback;

	void load_model(std::string model_name, float scale_x = 1.0f, float scale_y = 1.0f, float scale_z = 1.0f);
	void load_model(std::string model_name, std::vector<PlyVertex> vertices);
	void load_texture(std::string texture_name);
	void load_font(std::string font_name);
	void load_skybox(std::string directory, std::string file_pos_x, std::string file_neg_x, std::string file_pos_y, std::string file_neg_y, std::string file_pos_z, std::string file_neg_z);
	void load_particle(std::string texture_name, unsigned int max_particles);

	std::unordered_map<std::string, ModelInfo::ptr> models;
	std::unordered_map<std::string, unsigned int> textures;
	std::unordered_map<std::string, BumblebeeFontAtlas::ptr> fonts;
	std::unordered_map<std::string, ParticleInfo::ptr> particles;

private:
	ModelLoader::ptr _model_loader;

};

#endif