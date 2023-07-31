#ifndef HTTPMETHOD_HPP
#define HTTPMETHOD_HPP

#include "Client.hpp"
#include "../confPasser/LocBlock.hpp"
#include "ExtensionMimeMap.hpp"

#include <string>
#include <vector>
#include <unordered_map>
#include <fstream>

typedef enum e_method
{
	GET,
	HEAD,
	POST,
	DELETE
};

class HttpMethod {
	public:
		static HttpResponse *handleHttpMethod(HttpRequest* req);
		static HttpResponse *handleGet(HttpRequest* req);
		static HttpResponse *handleHead(HttpRequest* req);
		static HttpResponse *handlePost(HttpRequest* req);
		static HttpResponse *handleDelete(HttpRequest* req);

		static void writeVectorToFile(const std::string& filename, const std::vector<char>& content);
		static std::string isEmptyPath(std::string path);
	private:
};

#endif