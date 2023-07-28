#include <string>
#include <vector>
#include <map>

typedef enum method
{
	GET,
	POST,
	DELETE
} e_method;

static const std::map<std::string, std::string> mimeTypes = {
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

getMimeByExtension(std::string extension) {
	for (std::map<std::string, std::string>::const_iterator it = mimeTypes.begin(); it != mimeTypes.end(); it++) {
		if (it->first == extension)
			return it->second;
	}
	return "text/plain";
}

getExtensionByMime(std::string mime) {
	for (std::map<std::string, std::string>::const_iterator it = mimeTypes.begin(); it != mimeTypes.end(); it++) {
		if (it->second == mime)
			return it->first;
	}
	return "txt";
}

class HttpRequest {
	public :

	private :
		int			method_;
		std::string uri_path_;
		bool        valid_version_;

		std::map<std::string, std::string> headers_;
		std::vector<char> body_;
		int content_length_;
};

class HttpResponse {
	public: 
		char *toResponseMsg();
	private : 
		std::string 		fst_line_;
		std::map<std::string, std::string>	headers_;
		std::vector<char>	body_;

		int 		status_code_;
		std::string status_msg_;
		std::string version_ = "HTTP/1.1";

		int 		content_length_;
		int			content_type_;
};