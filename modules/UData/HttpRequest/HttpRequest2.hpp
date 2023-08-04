#ifndef HTTPREQUEST2_HPP
# define HTTPREQUEST2_HPP
# include <iostream>
# include <string>
# include <map>
# include <vector>
# include "../../config/Tools/ParseTool.hpp"

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
	VERSION_ERROR
}e_parseError;

class HttpRequest2{
	public :
		/* constructor */
		HttpRequest2();

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
		bool					parseBody(std::vector<char>& raw_data);
		bool					checkHeader(void);

		/* parsing utils */
		std::string		getLine(std::vector<char>& raw_data);
		std::string		getTarget(std::string& line);
		int						hexToDec(const std::string& base_num); 
};


#endif
