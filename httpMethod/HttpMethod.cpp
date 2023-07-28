#include "HttpMethod.hpp"

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
	std::ofstream file();
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
	std::ofstream file("test.txt");

	std::string s;

}

HttpResponse *HttpMethod::handleDelete(HttpRequest* req) {}