#ifndef _ModelLoader_H_
#define _ModelLoader_H_

#include "ModelInfo.h"
#include "FileReader.h"

#include <vector>
#include <iostream>

// Loads a model file (currently only ply)
class ModelLoader {
public:
	typedef std::shared_ptr<ModelLoader> ptr;

	ModelLoader();
	~ModelLoader();

	bool load_ply(const char* path, ModelInfo::ptr info);

private:
	FileReader::ptr _reader;

};

#endif