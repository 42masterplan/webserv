#ifndef HTTPRESPONSEHANDLER_HPP
# define HTTPRESPONSEHANDLER_HPP
# include "../UData/UData.hpp"
# include "../config/ConfParser.hpp"
# include "Cgi.hpp"
class HttpResponseHandler{
	public:
		static HttpResponseHandler&	getInstance();
		void	parseResponse(UData *udata);
		std::string convertToStr(e_method method);
		bool 	isDenyMethod(UData &udata, e_method method);
		void	handleResponse(UData *udata);
		void	errorCallBack(UData &udata, int status_code);
		void	RegisterClientWriteEvent(UData &udata);
		void	RegisterFileWriteEvent(int file_fd, UData &udata);
		void	RegisterFileReadEvent(int file_fd, UData &udata);
	private:
		void	handleHttpMethod(UData &udata);
		void	handleGet(UData &udata);
		void	handleHead(UData &udata);
		void	handlePost(UData &udata);
		void	handleDelete(UData &udata);
};

#endif
