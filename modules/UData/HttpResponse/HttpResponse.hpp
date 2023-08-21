#ifndef HTTP_RESPONSE_HPP
# define HTTP_RESPONSE_HPP

# include "../../config/ConfParser.hpp"
# include "../HttpRequest/HttpRequest.hpp"
# include "./Features/AutoIndex.hpp"
# include "./Store/StatusMsgStore.hpp"
# include "./Store/MimeStore.hpp"
# include "./Features/Session.hpp"


/* stat func */
typedef enum res_type {
	METHOD_TYPE,
	CGI_EXEC,
	AUTOINDEX,
	REDIRECT,
	ERROR
} e_res_type;

class  HttpResponse{
	public :
		HttpResponse();
		HttpResponse(HttpRequest &req);
		HttpResponse& operator=(const HttpResponse &ref);
		std::map<int, std::string> status_store_;

		/* methods */
		void		processErrorRes(int status_code);
		void		processDefaultErrorRes(int status_code);
		void		processRedirectRes(int status_code);
		bool		makeCgiResponse();
		void		makeBodyResponse();
		
		/* getter, setter */
		const std::string&				getFilePath() const;
		void											setFilePath(HttpRequest &req, LocBlock &loc);
		void											setLocation(std::string location);

    void                      setFileSize(const std::string& file_path);
		void											setStatusCode(int status_code);
		void											setContentLength(int content_length);

		std::vector<char>&				getBody();
		std::vector<char>& 				getJoinedData() ;
		std::string 							getErrorPath(int status_code);
		
		std::string 			http_version_;
		int								status_code_;
		std::string				status_;
		/* headers */
		int					content_length_;
		std::string	content_type_;
		std::string	location_;
		bool				exist_session_;
		LocBlock		loc_block_;

		std::vector<char>	joined_data_;//여기에 HTTP message를 담고(first line and headers)
		std::vector<char>	body_;//여기에 파일 또는 response body를 담겠습니다.

		e_res_type				res_type_;
		std::string				file_path_;
		int								client_fd_;
		int								write_size_;
    long              file_size_;
		bool							header_complete_;
		StatusMsgStore		status_msg_store_;
	private :
};

#endif
