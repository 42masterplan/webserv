#ifndef HTTPRESPONSE_HPP
# define HTTPRESPONSE_HPP

# include "../../config/LocationBlock/LocBlock.hpp"
# include "../../config/ConfParser.hpp"
# include "../.hpp" // for cgi

# include "HttpRequest.hpp"
# include "StatusMsgStore.hpp"
# include "MimeStore.hpp"

# include <iostream>
# include <fstream>
# include <string>
# include <vector>
# include <map>
# include <algorithm>

typedef enum e_res_type {
	UPLOAD_STORE,
	CGI,
	REDIRECT,
	ERROR
} e_res_type;


class HttpResponse{

	public :
		static std::map<int, std::string> status_store_;
		
		/* constructor */
		HttpResponse();
		HttpResponse(HttpRequest &req);
		void		makeResponse(HttpRequest &request);


		/* methods */
		void 		initStatusStore(void);
		void 		processDefaultErrorRes(HttpResponse &res, int status_code);
		void 		processRedirectRes(HttpResponse &res, int status_code);

		void	setFilePath(HttpRequest &req, LocBlock &loc);
		std::string &getFilePath(LocBlock &loc);
		
	private :
		std::string 			http_version_;
		int								status_code_;
		std::string				status_;
		/* headers */
		int					content_length_;
		std::string	content_type_;
		std::string	location_;

		std::vector<char>	body_;
		std::vector<char>	joined_data_;
		LocBlock					&loc_block_;
		e_res_type				res_type_;
		std::string				file_path_;
};

#endif