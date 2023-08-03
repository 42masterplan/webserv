#include "HttpMethod.hpp"

void HttpMethod::handleHttpMethod(UData &udata) {
	switch(udata.http_request_.getMethod()) {
		case GET:
			return handleGet(udata);
		case HEAD:
			return handleHead(udata);
		case POST:
			return handlePost(udata);
		case DELETE:
			return handleDelete(udata);
		default:
			return processErrorRes(udata, 405);
	}
}

void HttpMethod::handleGet(UData &udata) {
	HttpResponse res = udata.http_response_;
	std::ifstream in("test.txt");
	std::vector<char> body;

	if (!in)
		processErrorRes(udata, 404);
	in >> body;

	res.setStatusCode(200);
	res.setStatus(STATUS[200]);
	res.setBody();
}

void HttpMethod::handleHead(UData &udata) {

}

HttpMethod::handlePost(UData &udata) {
	// 확장자와 content-type 충돌 시 content-type 우선
	std::string filename = getCombineUploadStorePath(req->getPath()) + getExtension(req->getContentType());
	std::ofstream out(filename.c_str());
	std::string body = req->getBody();

	out.write(body.c_str(), body.size());
	out.close();
}

HttpMethod::handleDelete(UData &udata) {
	
}

