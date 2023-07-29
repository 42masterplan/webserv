#ifndef UDATA_HPP
# define UDATA_HPP
# include <iostream>
# include <string>
# include <vector>
# include "HttpRequest.hpp"
# include "HttpResponse.hpp"

typedef enum errorType{
	OK = 0,
}e_error;

class UData{
	public :
		/* constructor */
		UData();

		/* variables */
		HttpRequest				http_request_;
		HttpResponse			http_response_;
		e_error 					error_flag_;
		std::vector<char>	raw_data_;

		/* cgi */
		std::string				prog_name_;
		std::vector<char>	cgi_store_;

	private :
  
};

#endif