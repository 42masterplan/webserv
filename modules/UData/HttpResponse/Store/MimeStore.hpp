#ifndef EXTENSION_MIME_STORE_HPP
#define EXTENSION_MIME_STORE_HPP

#include <map>
#include <string>
#include <iostream>
class MimeStore {
	public:
		static void initMimeStore();
		static std::string getMime(std::string extension);
		static std::string getExtension(std::string mime);

		static std::map<std::string, std::string> mime_store_;
	
	private:
		MimeStore();
};

#endif
