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
		std::vector<HttpRequest>	http_requests_;
		std::vector<HttpResponse>	http_responses_;
		e_error 									error_flag_;
		std::vector<char>					raw_data_;

		/* cgi */
		std::string				prog_name_;
		std::vector<char>	cgi_store_;

	private :
  
};

#endif