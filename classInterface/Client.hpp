#include "CGI.hpp"
#include <vector>
class HttpRequest{
  public :

  private :
    std::string method_;
    std::string uri_path_;
    bool        valid_version_;

    std::map<std::string, std::string> headers_;
    std::vector<char> body_;
		int content_length_;
};

class HttpResponse{
  public: 
	char *toResponseMsg();
  private : 
    std::string       fst_line_;
    std::string       headers_;
    std::vector<char> body_;
};

class HttpParser{
  public :
    
  private :
};

class Client{
	public : 
    void  parseMessage(char* buff);
    /** 첫 줄일 때.
     * 1. buff CRLF가 있는지 검사
     * 2. 버퍼에 들어온 데이터를 raw_data_에 넣어준다
     * 3. 
     */
  private :
		int clnt_sock_fd_;
		std::vector<char> raw_data_; //default resize가능
    CGI clnt_cgi_;
		HttpRequest http_request_;
};
