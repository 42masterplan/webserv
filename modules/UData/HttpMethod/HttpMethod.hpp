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
	DELETE,
	POST,
	PUT,
	PATCH,
	OTHER_METHOD
};

class HttpMethod {
	public:
		static void handleHttpMethod(UData &udata);
		static void handleGet(UData &udata);
		static void handleHead(UData &udata);
		static void handlePost(UData &udata);
		static void handleDelete(UData &udata);

	private:
};

#endif