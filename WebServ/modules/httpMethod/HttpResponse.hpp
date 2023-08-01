#ifndef HTTPRESPONSE_HPP
# define HTTPRESPONSE_HPP
# include "LocBlock.hpp"
# include <iostream>
# include <string>
# include <vector>
# include <map>

class HttpResponse{

	public :
		static std::map<int, std::string> status_store_;
		
		/* constructor */
		HttpResponse();

		/* methods */
		void 		processDefaultErrorRes(HttpResponse &res, int status_code);
		void 		processDefaultRedirectRes(HttpResponse &res, int status_code);
		
	private :
		const std::string http_version_ = "HTTP/1.1";
		int								status_code_;
		std::string				status_;
		/* headers */
		int					content_length_;
		std::string	content_type_;
		std::string	location_;

		std::vector<char>	body_;
		std::vector<char>	joined_data_;
		LocBlock					loc_block_;
};

#endif