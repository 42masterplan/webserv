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

typedef enum parseStatusType{
	FIRST,
	HEADER,
	BODY,
	FINISH
}e_parseStatus;

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
		void		parse(std::vector<char>& raw_data);

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
		e_parseStatus	parse_status_;
		e_parseError	parse_error_;
		
		/* parsing functions */
		void					parseFirstLine(std::string line);
		void					parseHeader(std::string line);
		void					checkHeader(void);

		/* parsing utils */
		size_t				findCRLF(const std::vector<char>& raw_data) const;
		bool					hasCRLF(const std::vector<char>& raw_data) const;
		std::string		getLine(std::vector<char>& raw_data);
		std::string		getTarget(std::string& line);
		std::string		trimString(std::string& str) const;
		std::string		lowerString(std::string& str) const;
		bool					insensitiveCompare(std::string& str1, std::string& str2) const;
};

#endif