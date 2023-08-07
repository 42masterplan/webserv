#ifndef HTTP_METHOD_HPP
#define HTTP_METHOD_HPP

#include "../UData.hpp"

#include <string>
#include <vector>
#include <map>
#include <fstream>

typedef enum e_method {
	GET,
	HEAD,
	POST,
	DELETE
};

class HttpMethod {
	public:
		static void handleHttpMethod(UData &udata, HttpRequest &req, HttpResponse &res);
		static void handleGet(UData &udata, HttpRequest &req, HttpResponse &res);
		static void handleHead(UData &udata, HttpRequest &req, HttpResponse &res);
		static void handlePost(UData &udata, HttpRequest &req, HttpResponse &res);
		static void handleDelete(UData &udata, HttpRequest &req, HttpResponse &res);

	private:
};

#endif