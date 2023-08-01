#ifndef UDATA_HPP
# define UDATA_HPP
# include <iostream>
# include <string>
# include <vector>
// # include "../interface/HttpRequest.hpp"
// # include "../interface/HttpResponse.hpp"

typedef enum errorType{
	OK = 0
}e_error;

typedef enum fdType{
	SERVER,
	CLNT,
	CGI
}e_fd_type;

class UData{
	public :
		/* constructor */
		UData(e_fd_type type);
		~UData();

		/* variables */
		e_fd_type					fd_type_;
		// HttpRequest				http_request_;
		// HttpResponse			http_response_;
		e_error 					error_flag_;
		std::vector<char>	raw_data_;

		/* cgi */
		std::string				prog_name_;
		std::vector<char>	cgi_store_;
	private :
};

#endif
