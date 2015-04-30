#ifndef _Logger_H_
#define _Logger_H_

#include <memory>
#include <iostream>
#include <string>

// Provides global logging functionality
class Logger {
public:
	typedef std::shared_ptr<Logger> ptr;

	static ptr get();
	virtual ~Logger() { };

	void log(std::string text);

private:
	Logger() { };
	Logger(const Logger&) = delete;
	void operator=(const Logger&) = delete;
	static ptr _instance;

};

#endif