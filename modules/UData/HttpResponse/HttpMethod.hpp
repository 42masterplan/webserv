#ifndef HTTP_METHOD_HPP
#define HTTP_METHOD_HPP

#include "Udata.hpp"

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
		static void 

		static void handleHttpMethod(UData& udata);
		static void handleGet(UData& udata);
		static void handleHead(UData& udata);
		static void handlePost(UData& udata);
		static void handleDelete(UData& udata);

	private:
};

#endif