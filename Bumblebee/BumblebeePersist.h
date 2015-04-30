#ifndef _BumblebeePersist_H_
#define _BumblebeePersist_H_

#include <memory>
#include <string>
#include <iostream>

#include "BumblebeeObject.h"
#include "SpatialComponent.h"
#include "ModelComponent.h"

#include "Dependencies/include/sqlite/sqlite3.h"

struct ObjectInfo {
	std::string model_path;
	float x; float y; float z;
};

class BumblebeePersist {
public:
	typedef std::shared_ptr<BumblebeePersist> ptr;

	static ptr get();
	virtual ~BumblebeePersist() { };

	bool persist_to_json(BumblebeeObject* obj, std::string filename);
	ObjectInfo load_from_json(std::string filename);

	void set_high_score(int player_id, int score);
	int get_high_score(int player_id);
	std::string send_http_request(std::string request);
	std::vector<std::pair<int, int>> get_top_20();

	void test();

private:
	BumblebeePersist() { };
	BumblebeePersist(const BumblebeePersist&) = delete;
	void operator=(const BumblebeePersist&) = delete;
	static ptr _instance;

};

#endif