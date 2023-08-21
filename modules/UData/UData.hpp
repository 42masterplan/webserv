#ifndef UDATA_HPP
# define UDATA_HPP

# include "./HttpResponse/HttpResponse.hpp"
# include "./HttpRequest/HttpRequest.hpp"

typedef enum fdType{
	CLNT,
	CGI,
	FILETYPE
}e_fd_type;

class UData{
	public :
		/* constructor */
		UData(e_fd_type type, int port);
		~UData();

		/* variables */
		e_fd_type					fd_type_;
		std::vector<HttpRequest>	http_request_;
		HttpResponse	http_response_;
		std::vector<char>					raw_data_;

		/* cgi */
		std::string				prog_name_;
		std::vector<char>	cgi_store_;
		pid_t							cgi_pid_;
    int               r_pfd;
    int               w_pfd;

		/*file*/
		int								client_fd_;
		int								write_size_;
		int								port_;
		bool							is_forked_;
	private :
		UData();
};
#endif
