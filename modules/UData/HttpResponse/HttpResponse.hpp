#ifndef HTTPRESPONSE_HPP
# define HTTPRESPONSE_HPP

# include "../../config/LocationBlock/LocBlock.hpp"
# include "../../config/ConfParser.hpp"
# include "../../webserv/Cgi.hpp"

# include "../HttpMethod/HttpMethod.hpp"
# include "../HttpRequest/HttpRequest.hpp"
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
		HttpResponse(UData &udata, HttpRequest &req);
		HttpResponse& operator=(const HttpResponse &ref);

		/* methods */
		void 		initStatusStore(void);

		void		processErrorRes(int status_code);
		void 		processDefaultErrorRes(int status_code);
		void 		processRedirectRes(int status_code);


		/* getter, setter */

		const std::string&				getFilePath() const;
		void											setFilePath(HttpRequest &req, LocBlock &loc);

		LocBlock&									getLocBlock();

		void 											setStatusCode(int status_code);
		std::vector<char>&				getBody();
		const std::vector<char>& 	getJoinedData() const;
		void											setJoinedData(const std::vector<char> &joined_data);
		
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
		int								client_fd_;
		int								write_size_;
};

#endif