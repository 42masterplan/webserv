#ifndef HTTPRESPONSEHANDLER_HPP
# define HTTPRESPONSEHANDLER_HPP
# include "../UData.hpp"
# include "../../config/ConfParser.hpp"
# include "../../webserv/Cgi.hpp"
# include "../../webserv/AutoIndex.hpp"
class HttpResponseHandler{
	public:
		static HttpResponseHandler&	getInstance();
		void	parseResponse(UData *udata);
		std::string getErrorPage(int status_code);
		std::string convertToStr(e_method method);
		void	handleResponse(UData *udata);
		void	errorCallBack(UData &udata, int status_code);
	private:
		void	handleHttpMethod(UData &udata);
		void	handleHeadGet(UData &udata);
		void	handleHead(UData &udata);
		void	handlePost(UData &udata);
		void	handleDelete(UData &udata);

};

#endif
