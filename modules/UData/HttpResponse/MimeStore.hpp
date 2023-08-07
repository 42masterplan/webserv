#ifndef EXTENSION_MIME_STORE_HPP
#define EXTENSION_MIME_STORE_HPP

#include <map>
#include <string>

class MimeStore {
	public:
		static void initMimeStore();
		static std::string getMime(std::string extension);
		static std::string getExtension(std::string mime);

		static std::map<std::string, std::string> mime_store_;
};

#endif