#ifndef HTTPRESPONSE_HPP
# define HTTPRESPONSE_HPP
# include <iostream>
# include <string>
# include <vector>

class HttpResponse{
	public :
		/* constructor */
		HttpResponse();

		/* getter, setter */
		int								getStatusCode(void);
		std::string				getMessage(void);
		std::string 			getHttpVersion(void);
		std::vector<char>	getBody(void);
		int					getContentLength(void);
		std::string	getContentType(void);
		std::string	getLocation(void);

		void	setStatusCode(int status_code);
		void	setMessage(std::string message);
		void	setBody(std::vector<char> body);
		void	setContentLength(int content_length);
		void	setContentType(std::string content_type);
		void	setLocation(std::string location);

		/* methods */
		void 							clear();
		std::vector<char> toString();

	private :
		int								status_code_;
		std::string				message_;
		const std::string http_version_ = "HTTP/1.1";
		std::vector<char>	body_;

		/* headers */
		int					content_length_;
		std::string	content_type_;
		std::string	location_;
};

#endif