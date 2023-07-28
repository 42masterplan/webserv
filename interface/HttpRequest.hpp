#ifndef HTTPREQUEST_HPP
# define HTTPREQUEST_HPP
# include <iostream>
# include <string>
# include <map>
# include <vector>

typedef enum methodType{
	GET,
	HEAD,
	DELETE,
	POST,
	PUT,
	PATCH
}e_method;

class HttpRequest{
	public :
		/* constructor */
		HttpRequest();

		/* getter, setter */
		const e_method&			getMethod(void);
		const std::string&	getPath(void);
		const std::map<std::string, std::string>&	getHeader(void);
		const std::vector<char>&	getBody(void);
		const int&					getContentLength(void);
		const int&					getPort(void);
		const bool&					getIsChunked(void);
		const std::string&	getContentType(void);

		void	setMethod(e_method method);
		void	setPath(std::string path);
		void	setHeader(std::map<std::string, std::string> header);
		void	setBody(std::vector<char> body);
		void	setContentLength(int content_length);
		void	setPort(int port);
		void	setIsChunked(bool is_chunked);
		void	setContentType(std::string content_type);

		/* methods */
		void clear();

	private :
		e_method		method_;
		std::string	path_;
		std::map<std::string, std::string>	header_;
		std::vector<char>	body_;

		/* headers */
		int					port_;
		bool				is_chunked_;
		int					content_length_;
		std::string	content_type_;
};

#endif