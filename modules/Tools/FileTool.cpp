#include "FileTool.hpp"

bool	isExistFile(std::string filePath) {
	std::ifstream file(filePath.c_str());
	return file.good();
}
