#include "Logger.h"

Logger::ptr Logger::_instance;

// Returns instance
Logger::ptr Logger::get() {
	if (!_instance) {
		_instance = ptr(new Logger());
	}
	return _instance;
}

// Logs something, probably change this to render text later
void Logger::log(std::string text) {
	std::cout << text << std::endl;
}