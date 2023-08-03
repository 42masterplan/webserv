#include "ExtensionMimeMap.hpp"

std::string ExtensionMimeTypeMap::getMime(std::string extension) {
	for (std::map<std::string, std::string>::const_iterator it = EXTENSION_MIME_MAP.begin(); it != EXTENSION_MIME_MAP.end(); it++) {
		if (it->first == extension)
			return it->second;
	}
	return "text/plain";
}

std::string ExtensionMimeTypeMap::getExtension(std::string mime) {
	for (std::map<std::string, std::string>::const_iterator it = EXTENSION_MIME_MAP.begin(); it != EXTENSION_MIME_MAP.end(); it++) {
		if (it->second == mime)
			return it->first;
	}
	return "txt";
}
