#include "HttpMethod.hpp"

HttpResponse *HttpMethod::handleHttpMethod(HttpRequest* request) {
	switch(request->getMethod()) {
		case GET:
			return handleGet(request);
		case POST:
			return handlePost(request);
		case DELETE:
			return handleDelete(request);
		default:
			return NULL;
	}
}

HttpResponse *HttpMethod::handleGet(HttpRequest* request) {
	
}

HttpResponse *HttpMethod::handlePost(HttpRequest* request) {}

HttpResponse *HttpMethod::handleDelete(HttpRequest* request) {}