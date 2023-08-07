#include "MimeStore.hpp"
#include <iostream>

std::map<std::string, std::string> MimeStore::mime_store_;

void MimeStore::initMimeStore() {
	mime_store_[".txt"] = "text/plain";
	mime_store_[".html"] = "text/html";
	mime_store_[".css"] = "text/css";
	mime_store_[".js"] = "text/javascript";
	mime_store_[".jpg"] = "image/jpeg";
	mime_store_[".png"] = "image/png";
	mime_store_[".gif"] = "image/gif";
	mime_store_[".bmp"] = "image/bmp";
	mime_store_[".mp3"] = "audio/mpeg";
	mime_store_[".wav"] = "audio/wav";
	mime_store_[".ogg"] = "audio/ogg";
	mime_store_[".mp4"] = "video/mp4";
	mime_store_[".mov"] = "video/quicktime";
	mime_store_[".webm"] = "video/webm";
	mime_store_[".zip"] = "application/zip";
	mime_store_[".rar"] = "application/x-rar-compressed";
	mime_store_[".tar"] = "application/x-tar";
	mime_store_[".pdf"] = "application/pdf";
	mime_store_[".doc"] = "application/msword";
	mime_store_[".xls"] = "application/vnd.ms-excel";
	mime_store_[".ppt"] = "application/vnd.ms-powerpoint";
}

std::string MimeStore::getMime(std::string extension) {
	for (std::map<std::string, std::string>::const_iterator it = mime_store_.begin(); it != mime_store_.end(); it++) {
		if (it->first == extension)
			return it->second;
	}
	return "not supported";
}

std::string MimeStore::getExtension(std::string mime) {
	if (mime_store_.empty())
		initMimeStore();
	for (std::map<std::string, std::string>::const_iterator it = mime_store_.begin(); it != mime_store_.end(); it++) {
		if (it->second == mime)
			return it->first;
	}
	return "txt";
}
