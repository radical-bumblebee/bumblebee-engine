#ifndef _BumblebeePersist_H_
#define _BumblebeePersist_H_

#include <memory>
#include <string>
#include <iostream>

#include "BumblebeeObject.h"
#include "SpatialComponent.h"
#include "ModelComponent.h"

#include "Dependencies/include/sqlite/sqlite3.h"

// Provides persistence functionality
class BumblebeePersist {
public:
	typedef std::shared_ptr<BumblebeePersist> ptr;

	static ptr get();
	virtual ~BumblebeePersist() { };

	void set_high_score(int player_id, int score);
	int get_high_score(int player_id);
	std::string send_http_request(std::string request);
	std::vector<std::pair<int, int>> get_top_20();

private:
	BumblebeePersist() { };
	BumblebeePersist(const BumblebeePersist&) = delete;
	void operator=(const BumblebeePersist&) = delete;
	static ptr _instance;

};

#endif