#ifndef HTTPREQUEST_HPP
# define HTTPREQUEST_HPP
# include <iostream>
# include <string>
# include <map>
# include <vector>
# include "../../Tools/ParseTool.hpp"
# include "../../Event/Session.hpp"

typedef enum parseStatusType{
	FIRST,
	HEADER,
	BODY,
	FINISH
}e_parseStatus;

typedef enum requestErrorType{
	OK = 0,
	FORM_ERROR,
	METHOD_ERROR,
	VERSION_ERROR,
	UNIMPLEMENTED_ERROR,
	LENGTH_REQUIRED_ERROR
}e_requestError;

class HttpRequest{
	public :
		/* constructor */
		HttpRequest();

		/* getter, setter */
		const e_method&						getMethod(void) const;
		const std::string&				getPath(void) const;
		const std::map<std::string, std::string>&	getHeader(void) const;
		const std::vector<char>&	getBody(void) const;
		const int&								getContentLength(void) const;
		const int&								getPort(void) const;
		const bool&								getIsChunked(void) const;
		const std::string&				getContentType(void) const;
		const std::string&				getHost(void) const;
		const e_requestError&			getRequestError(void) const;
		const e_parseStatus&			getParseStatus(void) const;
		void											setPort(int port);

		void	printRequestInfo();

		/* methods */
		void		clear();
		void		parse(std::vector<char>& raw_data);
		void		printBodyInfo();//for test

	private :
		e_method				method_;
		std::string			path_;
		std::map<std::string, std::string>	header_;
		std::vector<char>	body_;

		/* headers */
		int							port_;
		bool						is_chunked_;
		bool						exist_session_;
		int							content_length_;
		std::string			content_type_;
		std::string			host_;
		std::map<std::string, std::string>	cookie_;

		/* parsing */
		e_parseStatus		parse_status_;
		e_requestError	request_error_;
		std::string			last_header_;

		/* body parsing member */
		bool read_state_;
		int to_read_;

		/* parsing functions */
		void						parseFirstLine(std::string line);
		void						parseHeader(std::string line);
		void						checkHeader(void);
		bool						parseBody(std::vector<char>& raw_data);
		bool						parseChunkedBody(std::vector<char>& raw_data);

		/* parsing utils */
		std::string			getLine(std::vector<char>& raw_data);
		std::string			getTarget(std::string& line);
		static const std::map<std::string, bool>	get_multiple_header();
		int							hexToDec(const std::string& base_num);
};
void	print_vec(const std::vector<char>& t);
#endif
