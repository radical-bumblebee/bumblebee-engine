#ifndef _FileReader_H_
#define _FileReader_H_

#include <stdio.h>
#include <string>
#include <memory>

#define MAX_LENGTH 1024

// Utility class to assist in file parsing
class FileReader {
public:
	typedef std::shared_ptr<FileReader> ptr;

	FileReader() { };
	FileReader(const char* src) :
		_src(src) { };
	~FileReader() { };

	// Sets the source buffer
	inline void set(const char* src) {
		_src = src;
	}

	// Check if we've reached the end of the file
	inline bool eof() {
		return (0 == *_src);
	}

	// Eats characters to the end of the line
	void eat_line() {
		//if (0 == *_src) {
			if ((isalnum(*_src) || *_src == '.' || *_src == '-')) {
				return;
			}
			// To line ending
			while (!eof() && !eol(*_src)) {
				_src++;
			}
			// Consume all line ending characters
			while (!eof() && eol(*_src)) {
				_src++;
			}
		//}
	}

	// Reads to next space
	bool read(char* out) {
		if (eof()) { return false; };
		memset(out, 0, MAX_LENGTH);

		int idx = 0;

		while (isalnum(*_src) || *_src == '.' || *_src == '-') {
			out[idx] = *_src;
			_src++;
			idx++;
		}

		_src++;

		return true;
	}


private:
	const char* _src;

	// Checks if we're at a line ending
	inline bool eol(const char c) {
		return (c == '\n' || c == '\r');
	}


};

#endif