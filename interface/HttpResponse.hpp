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
		const int&					getStatusCode(void) const;
		const std::string&	getStatusMessage(void) const;
		const std::string& getHttpVersion(void) const;
		const std::vector<char>&	getBody(void) const;
		const std::string&	getJoinedData(void) const;
		const int&					getContentLength(void) const;
		const std::string&	getContentType(void) const;
		const std::string&	getLocation(void) const;

		void	setStatusCode(int status_code);
		void	setStatusMessage(std::string message);
		void	setBody(std::vector<char> body);
		void	setJoinedData(std::string joined_data);
		void	setContentLength(int content_length);
		void	setContentType(std::string content_type);
		void	setLocation(std::string location);

		/* methods */
		void 							clear();
		std::vector<char> toString();

	private :
		int								status_code_;
		std::string				status_message_;
		const std::string http_version_ = "HTTP/1.1";
		std::vector<char>	body_;
		std::string				joined_data_;

		/* headers */
		int					content_length_;
		std::string	content_type_;
		std::string	location_;
};

#endif