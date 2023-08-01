#ifndef HTTPREQUEST_HPP
# define HTTPREQUEST_HPP
# include <iostream>
# include <string>
# include <map>
# include <vector>

//TODO: 조신님과 겹침 !
typedef enum methodType{
	GET,
	HEAD,
	DELETE,
	POST,
	PUT,
	PATCH
}e_method;

typedef enum parseStatus{
	FIRST,
	HEADER,
	BODY,
	FIN
}e_parse;

typedef enum parseErrorType{
	OK = 0,
	FORM_ERROR,
	METHOD_ERROR,
	VERSION_ERROR,
}e_parseError;

class HttpRequest{
	public :
		/* constructor */
		HttpRequest();

		/* getter, setter */
		const e_method&			getMethod(void) const;
		const std::string&	getPath(void) const;
		const std::map<std::string, std::string>&	getHeader(void) const;
		const std::vector<char>&	getBody(void) const;
		const int&					getContentLength(void) const;
		const int&					getPort(void) const;
		const bool&					getIsChunked(void) const;
		const std::string&	getContentType(void) const;

		/* methods */
		void		clear();
		e_error parse(char* buff, size_t len, std::vector<char>& raw_data);

		
		// void	setMethod(e_method method);
		// void	setPath(std::string path);
		// void	setHeader(std::map<std::string, std::string> header);
		// void	setBody(std::vector<char> body);
		// void	setContentLength(int content_length);
		// void	setPort(int port);
		// void	setIsChunked(bool is_chunked);
		// void	setContentType(std::string content_type);

	private :
		e_method		method_;
		std::string	path_;
		std::map<std::string, std::string>	header_;
		std::vector<char>	body_;

		/* headers */
		int						port_;
		bool					is_chunked_;
		int						content_length_;
		std::string		content_type_;

		/* parsing */
		e_parse				parse_status_;
		e_parseError	parse_error_;
		
		/* parsing functions */
		void					parseFirstLine(std::string& line);
		void					parseHeader(std::string& line);

		/* parsing utils */
		size_t				findCRLF(const std::vector<char>& raw_data) const;
		bool					hasCRLF(const std::vector<char>& raw_data) const;
		std::string&	getLine(std::vector<char>& raw_data) const;
		std::string&	getTarget(std::string& line) const;

};

#endif