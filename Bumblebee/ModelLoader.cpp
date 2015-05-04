#include "ModelLoader.h" 

// Parses a ply file from a source path into a passed instance of ModelInfo
bool ModelLoader::load_ply(const char* path, ModelInfo::ptr info) {
	FILE* file = nullptr;
	if (fopen_s(&file, path, "rb") != 0) {
		std::cout << "Can't open model file" << std::endl;
		return false;
	}

	// Gets file length
	int length = 0;
	fseek(file, 0, SEEK_END);
	length = ftell(file);
	fseek(file, 0, SEEK_SET);

	// Reads file
	char* src = new char[length + 1];
	fread(src, 1, length, file);

	char buf[MAX_LENGTH];

	_reader->set(src);

	// Parses ply header
	_reader->read(buf);
	if (buf[0] != 'p' || buf[1] != 'l' || buf[2] != 'y') {
		std::cout << "Model file not a valid ply" << std::endl;
		return false;
	}

	info->name = path;

	int num_properties = 0;

	bool done_header = false;
	while (!done_header) {
		_reader->read(buf);
		switch (buf[0]) {
		case 'e':
			if (buf[1] == 'n' && buf[2] == 'd') {
				// end_header
				_reader->read(buf);
				done_header = true;
				break;
			}
			else if (buf[1] == 'l' && buf[2] == 'e' && buf[3] == 'm' && buf[4] == 'e' && buf[5] == 'n' && buf[6] == 't') {
				// element
				_reader->read(buf);
				switch (buf[0]) {
				case 'v':
					// vertices
					_reader->read(buf);
					info->num_vertices = (int)strtol(buf, NULL, 0);
					break;

				case 'f':
					// faces
					_reader->read(buf);
					info->num_faces = (int)strtol(buf, NULL, 0);
					break;
				}
			}
			break;
		case 'p':
			if (buf[1] == 'r' && buf[2] == 'o' && buf[3] == 'p') {
				_reader->read(buf);
				if (buf[0] == 'l' && buf[1] == 'i' && buf[2] == 's' && buf[3] == 't') {
					_reader->eat_line();
					continue;
				}
				num_properties++;
				_reader->eat_line();
			}
			break;
		}
	}

	// Parse ply vertices
	float* temp_prop = new float[num_properties]();
	for (int i = 0; i < info->num_vertices; ++i) {
		_reader->eat_line();

		for (int j = 0; j < num_properties; ++j) {
			_reader->read(buf);
			temp_prop[j] = static_cast<float>(atof(buf));
		}
		info->vertices.push_back(PlyVertex(temp_prop[0], temp_prop[1], temp_prop[2], temp_prop[3], temp_prop[4], temp_prop[5], temp_prop[6], temp_prop[7]));
	}
	delete[] temp_prop;

	// Parse ply faces
	int temp_face[FACES];
	for (int i = 0; i < info->num_faces; ++i) {
		_reader->eat_line();

		// Always 3, this only supports triangles
		if (!_reader->read(buf)) {
			break;
		}		

		for (int j = 0; j < FACES; ++j) {
			_reader->read(buf);
			temp_face[j] = (int)strtol(buf, NULL, 0);
		}
		info->faces.push_back(PlyFace(temp_face[0], temp_face[1], temp_face[2]));
	}

	delete[] src;

	return true;
}

ModelLoader::ModelLoader() {
	_reader = std::make_shared<FileReader>();
}

ModelLoader::~ModelLoader() {
	_reader.reset();
}