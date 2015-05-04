#include "BumblebeeProxy.h"

// Loads a ply model
void BumblebeeProxy::load_model(std::string model_name, float scale_x, float scale_y, float scale_z) {
	ModelInfo::ptr info = std::make_shared<ModelInfo>();
	if (!_model_loader->load_ply(model_name.c_str(), info)) {
		return;
	}

	// Scales it a bit
	for (auto it = info->vertices.begin(); it != info->vertices.end(); ++it) {
		it->x *= scale_x; it->y *= scale_y; it->z *= scale_z;
	}

	info->scale = glm::vec3(scale_x, scale_y, scale_z);
	
	// Signals callback
	this->models[info->name] = info;
	if (model_added_callback) {
		model_added_callback(info);
	}
}

void BumblebeeProxy::load_model(std::string model_name, std::vector<PlyVertex> vertices) {
	ModelInfo::ptr info = std::make_shared<ModelInfo>();
	info->vertices = vertices;
	info->num_vertices = vertices.size();
	info->scale = glm::vec3(1.0f);
	info->name = model_name;

	// Signals callback
	this->models[info->name] = info;
	if (model_added_callback) {
		model_added_callback(info);
	}
}

// Loads texture
void BumblebeeProxy::load_texture(std::string texture_name) {
	// Signals callback
	if (texture_added_callback) {
		textures[texture_name] = texture_added_callback(texture_name.c_str());
	}
}

// Loads font into a font atlas
void BumblebeeProxy::load_font(std::string font_name) {
	// Signals callback
	if (font_added_callback) {
		FT_Face face = font_added_callback(font_name.c_str());
		BumblebeeFontAtlas::ptr atlas = std::make_shared<BumblebeeFontAtlas>(face);
		fonts[font_name] = atlas;
	//	FT_Done_Face(face);
	}
}

// Loads skybox
void BumblebeeProxy::load_skybox(std::string directory, std::string file_pos_x, std::string file_neg_x, std::string file_pos_y, std::string file_neg_y, std::string file_pos_z, std::string file_neg_z) {
	if (skybox_added_callback) {
		textures["skybox"] = skybox_added_callback((directory + file_pos_x).c_str(), (directory + file_neg_x).c_str(), (directory + file_pos_y).c_str(), (directory + file_neg_y).c_str(), (directory + file_pos_z).c_str(), (directory + file_neg_z).c_str());
	}
}

// Loads particle information
void BumblebeeProxy::load_particle(std::string texture_name, unsigned int max_particles) {
	if (particle_added_callback) {
		ParticleInfo::ptr particle = std::make_shared<ParticleInfo>();
		particle->texture_id = textures[texture_name];
		particle->max_particles = max_particles;
		particles[texture_name] = particle;
		particle_added_callback(particle);
	}
}

bool BumblebeeProxy::init() {
	_model_loader = std::make_shared<ModelLoader>();

	return true;
}

void BumblebeeProxy::destroy() {
	_model_loader.reset();
}