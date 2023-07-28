#ifndef HTTPMETHOD_HPP
#define HTTPMETHOD_HPP

#include "Client.hpp"

class HttpMethod {
	public:
		static HttpResponse *handleHttpMethod(HttpRequest* request);
		static HttpResponse *handleGet(HttpRequest* request);
		static HttpResponse *handlePost(HttpRequest* request);
		static HttpResponse *handleDelete(HttpRequest* request);
	private:
};

#endif