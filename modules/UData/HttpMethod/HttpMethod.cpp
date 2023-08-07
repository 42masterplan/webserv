#include "HttpMethod.hpp"

static std::string convertToStr(e_method method) {
	switch(method) {
		case GET:
			return "GET";
		case HEAD:
			return "HEAD";
		case POST:
			return "POST";
		case DELETE:
			return "DELETE";
		default:
			return "";
	}
}

void HttpMethod::handleHttpMethod(UData &udata, HttpRequest &req, HttpResponse &res) {
	e_method method = req.getMethod();
	const std::vector<std::string> deny_method = res.getLocBlock().getDenyMethod();

	if (std::find(deny_method.begin(), deny_method.end(), convertToStr(method)) == deny_method.end())
		return res.processErrorRes(405);
	switch(method) {
		case GET:
			return handleGet(req, res);
		case HEAD:
			return handleHead(req, res);
		case POST:
			return handlePost(req, res);
		case DELETE:
			return handleDelete(req, res);
		default:
			return res.processErrorRes(405);
	}
}

void HttpMethod::handleGet(UData &udata, HttpRequest &req, HttpResponse &res) {
	FILE *file = fopen(res.getFilePath().c_str(), "r");
	if (file == NULL)
		return res.processErrorRes(404);
	Kqueue::registerReadEvent(file, );

	res.setStatusCode(200);
	res.setStatus(STATUS[200]);
	res.setBody();
}

void HttpMethod::handleHead(UData &udata, HttpRequest &req, HttpResponse &res) {
	std::ifstream in("test.txt");
	std::vector<char> body;

	if (!in)
		res.processErrorRes(404);
	in >> body;

	res.setStatusCode(200);
	res.setStatus(STATUS[200]);
	res.setBody();
}

HttpMethod::handlePost(UData &udata, HttpRequest &req, HttpResponse &res) {
	// 확장자와 content-type 충돌 시 content-type 우선
	std::string filename = getCombineUploadStorePath(req->getPath()) + getExtension(req->getContentType());
	std::ofstream out(filename.c_str());
	std::string body = req->getBody();

	out.write(body.c_str(), body.size());
	out.close();
}

HttpMethod::handleDelete(UData &udata, HttpRequest &req, HttpResponse &res) {
	
}

