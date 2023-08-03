#ifndef EXTENSION_MIME_MAP_HPP
#define EXTENSION_MIME_MAP_HPP

#include <map>
#include <string>

static std::map<std::string, std::string> EXTENSION_MIME_MAP = {
	{"txt", "text/plain"},
	{"html", "text/html"},
	{"css", "text/css"},
	{"js", "text/javascript"},
	{"jpg", "image/jpeg"},
	{"png", "image/png"},
	{"gif", "image/gif"},
	{"bmp", "image/bmp"},
	{"mp3", "audio/mpeg"},
	{"wav", "audio/wav"},
	{"ogg", "audio/ogg"},
	{"mp4", "video/mp4"},
	{"mov", "video/quicktime"},
	{"webm", "video/webm"},
	{"zip", "application/zip"},
	{"rar", "application/x-rar-compressed"},
	{"tar", "application/x-tar"},
	{"pdf", "application/pdf"},
	{"doc", "application/msword"},
	{"xls", "application/vnd.ms-excel"},
	{"ppt", "application/vnd.ms-powerpoint"},
};

class ExtensionMimeTypeMap {
	public:
		static std::string getMime(std::string extension);
		static std::string getExtension(std::string mime);
};

#endif