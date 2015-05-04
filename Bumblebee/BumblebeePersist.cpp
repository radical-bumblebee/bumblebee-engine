#include "BumblebeePersist.h"
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment (lib, "Ws2_32.lib")

BumblebeePersist::ptr BumblebeePersist::_instance;
int recv_msg(int sock, unsigned char** buffer);

std::string BumblebeePersist::send_http_request(std::string request) {
	const char* SERVER = "www.radbee.me";
	const char* PORT = "80";
	int error_code = 0;
	std::string response = "";

	WSADATA wsa_data;
	error_code = WSAStartup(MAKEWORD(2, 2), &wsa_data);
	if (error_code != 0) {
		printf("WSA startup failed");
		return response;
	}

	struct addrinfo *result = NULL;
	struct addrinfo	hints = { 0 };

	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	error_code = getaddrinfo(SERVER, PORT, &hints, &result);
	if (error_code != 0) {
		printf("getaddrinfo failed");
		printf(std::to_string(WSAGetLastError()).c_str());
		WSACleanup();
		return response;
	}

	SOCKET score_socket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	if (score_socket == INVALID_SOCKET) {
		printf("socket failed");
		freeaddrinfo(result);
		WSACleanup();
		return response;
	}

	if (connect(score_socket, result->ai_addr, static_cast<int>(result->ai_addrlen)) == SOCKET_ERROR) {
		printf("connect failed");
		freeaddrinfo(result);
		WSACleanup();
		return response;
	}

	send(score_socket, request.c_str(), request.length(), NULL);
	unsigned char* buffer;
	if (recv_msg(score_socket, &buffer) > 0) {
		response = (char*)buffer;
	}
	delete[] buffer;

	shutdown(score_socket, SD_SEND);
	closesocket(score_socket);
	freeaddrinfo(result);
	WSACleanup();

	return response;
}

std::vector<std::pair<int, int>> BumblebeePersist::get_top_20() {
	std::string request = "GET /api/scores/ HTTP/1.0\r\nHost: www.radbee.me\r\nConnection: close\r\n\r\n";
	std::string response = send_http_request(request);
	std::string id;
	std::string score;

	// Parse get request
	std::vector<std::pair<int, int>> scores;
	const char* response_char = response.c_str();
	while (*response_char != NULL) {
		id = ""; score = "";
		if (*response_char == '|') {
			++response_char;
			while (*response_char != '~') {
				id.push_back(*response_char);
				++response_char;
			}
			++response_char;
			while (*response_char != '|') {
				score.push_back(*response_char);
				++response_char;
			}
			scores.push_back({ std::stoi(id), std::stoi(score) }); // ya this is a real thing
		}
		++response_char;
	}

	return scores;
}


// Pushes high score of a specific player id to the db
void BumblebeePersist::set_high_score(int player_id, int score) {
	sqlite3* db;
	if (sqlite3_open_v2("assets/persist/BumblebeeDB.db", &db, SQLITE_OPEN_READWRITE, nullptr) != SQLITE_OK) {
		printf(sqlite3_errmsg(db));
	}

	std::string sql =
		"UPDATE OR IGNORE Player SET high_score='" + std::to_string(score) + "' WHERE id='" + std::to_string(player_id) + "';\n" \
		"INSERT OR IGNORE INTO Player (id, high_score)\n" \
		"VALUES(" + std::to_string(player_id) + ", " + std::to_string(score) + ");";		

	if (sqlite3_exec(db, sql.c_str(), nullptr, nullptr, nullptr) != SQLITE_OK) {
		printf(sqlite3_errmsg(db));
	}

	sqlite3_close_v2(db);
	
	// Keeping it local at this point

	/*std::string string_id = std::to_string(player_id);
	std::string string_score = std::to_string(score);
	std::string content_length = std::to_string(string_id.size() + string_score.size() + 10);

	std::string request =
	"POST /api/scores/ HTTP/1.0\r\n" \
	"User-Agent: HTTPTool/1.0\r\n" \
	"Content-Type: application/x-www-form-urlencoded\r\n" \
	"Content-Length: " + content_length + "\r\n\r\n" \
	"id=" + string_id + "&score=" + string_score;
	send_http_request(request);*/
}

// Retrieves high score for a specific player id
int BumblebeePersist::get_high_score(int player_id) {
	int score = 0;
	
	sqlite3* db;
	if (sqlite3_open_v2("assets/persist/BumblebeeDB.db", &db, SQLITE_OPEN_READONLY, nullptr) != SQLITE_OK) {
		printf(sqlite3_errmsg(db));
		return score;
	}

	std::string sql =
		"SELECT * FROM Player\n" \
		"WHERE id = " + std::to_string(player_id) + ";";

	sqlite3_stmt* stmt;
	sqlite3_prepare_v2(db, sql.c_str(), sql.size(), &stmt, nullptr);
	if (sqlite3_step(stmt) == SQLITE_ROW) {
		score = sqlite3_column_int(stmt, 1);
	}

	sqlite3_finalize(stmt);
	sqlite3_close_v2(db);

	return score;
}

int recv_msg(int sock, unsigned char** buffer) {
	int recv_bytes = 0;
	int received = 0;
	int length = 2000;

	*buffer = (unsigned char *)malloc(sizeof(unsigned char) * length);
	while (recv_bytes < length){
		received = recv(sock, (char*)*buffer + (sizeof(unsigned char) * recv_bytes), length - recv_bytes, 0);
		if (received < 0) {
			return received;
		}
		else if (received == 0) {
			auto x = *buffer;
			int i = 0;
			while (!isalnum(x[recv_bytes-i])) {
				i++;
			}
			x[recv_bytes-i+2] = '\0';
			return recv_bytes;
		}
		else {
			recv_bytes += (received + 4);
		}
	}

	return recv_bytes;
}

// Returns instance
BumblebeePersist::ptr BumblebeePersist::get() {
	if (!_instance) {
		_instance = ptr(new BumblebeePersist());
	}
	return _instance;
}