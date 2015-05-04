#include "Logger.h"

Logger::ptr Logger::_instance;

// Returns instance
Logger::ptr Logger::get() {
	if (!_instance) {
		_instance = ptr(new Logger());
	}
	return _instance;
}

// Logs something, could change this to render text or make a log file at some point
void Logger::log(std::string text) {
	std::cout << text << std::endl;
}