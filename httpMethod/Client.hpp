#include <string>
#include <vector>
#include <map>
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