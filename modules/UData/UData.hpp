#ifndef UDATA_HPP
# define UDATA_HPP
# include <iostream>
# include <string>
# include <vector>
# include "./HttpRequest/HttpRequest.hpp"
# include "./HttpResponse/HttpResponse.hpp"
class HttpResponse;
//OK가 HttpRequest file과 겹침
// typedef enum errorType{
// 	OK = 0
// }e_error;
typedef enum fdType{
	CLNT,
	CGI,
	FILETYPE
}e_fd_type;
class UData{
	public :
		/* constructor */
		UData(e_fd_type type);
		~UData();

		/* variables */
		e_fd_type					fd_type_;
		std::vector<HttpRequest>	http_request_;
		std::vector<HttpResponse>	http_response_;
		// e_error 									error_flag_;
		std::vector<char>					raw_data_;

		/* cgi */
		std::string				prog_name_;
		std::vector<char>	cgi_store_;
		pid_t							cgi_pid_;
		/*임시 결과 저장소 for test*/
		std::vector<char> ret_store_;//return 하는 값을 저장하는 저장소. 임시로 만듬
		/*file*/
		std::vector<char> file_read_write_store_;
		int 							status_code_;
		int								client_fd_;
		int								write_size_;
	private :
		UData();
};
#endif