#include "HttpMethod.hpp"

//map<int status_code, std::string status_msg>
std::unordered_map<int, std::string> static STATUS = {
	{200, "200 OK"},
	{201, "201 Created"},
	{202, "202 Accepted"},
	{204, "204 No Content"},
	{301, "301 Moved Permanently"},
	{302, "302 Found"},
	{304, "304 Not Modified"},
	{400, "400 Bad Request"},
	{401, "401 Unauthorized"},
	{403, "403 Forbidden"},
	{404, "404 Not Found"},
	{405, "405 Method Not Allowed"},
	{500, "500 Internal Server Error"},
	{501, "501 Not Implemented"},
	{502, "502 Bad Gateway"},
	{503, "503 Service Unavailable"}
};

static HttpResponse *handleError(int fd, int status_code) {
	std::string status = STATUS[status_code];

	std::string body = 
	"<html>\
		<head><title>" + status + "</title></head>\
		<body><h1>" + status + "</h1></body>\
	</html>";
	std::string header = 
	"HTTP/1.1 " + status + "\r\n\
	Content-Type: text/html\r\n\
	Content-Length: " + std::to_string(body.size()) + "\r\n\r\n";

	return 
}

static HttpResponse *handleRedirect(int fd, int status_code) {
	std::string status = STATUS[status_code];

	std::string body = 
	"<html>\
		<head><title>" + status + "</title></head>\
		<body><h1>" + status + "</h1></body>\
	</html>";
	std::string header = 
	"HTTP/1.1 " + status + "\r\n\
	Content-Type: text/html\r\n\
	Content-Length: " + std::to_string(body.size()) + "\r\n\r\n";
}

HttpResponse *HttpMethod::handleHttpMethod(HttpRequest* req) {
	switch(req->getMethod()) {
		case GET:
			return handleGet(req);
		case HEAD:
			return handleHead(req);
		case POST:
			return handlePost(req);
		case DELETE:
			return handleDelete(req);
		default:
			return NULL;
	}
}

HttpResponse *HttpMethod::handleGet(HttpRequest* req) {
	std::ifstream in("test.txt");
	std::string s;

	if (!in.is_open()) {
		handleError(req->fd, 404);
	}
	in >> s;

	return 0;
}

HttpResponse *HttpMethod::handleHead(HttpRequest* req) {}



static void writeVectorToFile(const std::string& filename, const std::vector<char>& content) {
    std::ofstream file(filename.c_str(), std::ios::binary);
    if (!file) {
        // 404 등등 에러메시지 루틴 std::cerr << "Error: Failed to open the file: " << filename << std::endl;
        return;
    }

    file.write(&content[0], content.size());
	file.close();
}


HttpResponse *HttpMethod::handlePost(HttpRequest* req) {
	// 확장자와 content-type 충돌 시 content-type 우선
	std::string filename = getCombineUploadStorePath(req->getPath()) + getExtension(req->getContentType());
	std::ofstream out(filename.c_str());
	std::string body = req->getBody();

	out.write(body.c_str(), body.size());
	out.close();
}

HttpResponse *HttpMethod::handleDelete(HttpRequest* req) {
	
}

