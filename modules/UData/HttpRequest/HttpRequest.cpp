#include "HttpRequest.hpp"

/**
 * 파싱 순서 정리 (new) *HttpRequest가 vector<HttpRequest>의 형태로 변함에 따라 파싱 알고리즘이 변화되었습니다.*
 * ---- event handler ----
 * 1. event를 handle하는 곳에서 raw_data에 buff를 더합니다.
 * 2. request가 없거나, 모든 request가 FINISH 상태일 때 새 HttpRequest를 생성합니다.
 * 3. FIN이 아닌 HttpRequest에 대해 raw_data를 인자로 parse 함수를 실행합니다.
 * ---- parse 함수 실행 ----
 * 주어진 raw_data에 대해 가능한 모든 파싱을 진행합니다.
 * 각 request마다 오류가 발생했을 시 flag를 설정한 뒤 종료합니다.
 * * FORM_ERROR: 400 Bad Request
 * * METHOD_ERROR: 405 Method Not Allowed
 * * VERSION_ERROR: 505 HTTP Version Not Supported
 * * UNIMPLEMENTED_ERROR: 501 Unimplemented
 * * LENGTH_REQUIRED_ERROR: 411 Length Required
 * ---- parse 함수 끝 ----
 * 마지막 HttpRequest가 FINISH이고 오류가 없으며, raw_data가 비어있지 않을 시 다음 HttpRequest 파싱을 진행합니다.
 * ---- 전체 파싱 끝 ----
 * getRequestError()를 통해 각 HttpRequest마다 오류 여부를 확인하고, 오류 발생 시 오류 response 생성을 위해 분기합니다.
 * status가 FINISH인 HttpRequest에 대해 HttpResponse를 생성한 뒤 해당 HttpRequest를 vector에서 pop합니다.
 */

HttpRequest::HttpRequest(): path_(""), is_chunked_(false), exist_session_(false), content_length_(-1), content_type_(""), \
host_(""), parse_status_(FIRST), request_error_(OK), last_header_(""), read_state_(false), to_read_(0) { }

const e_method&						HttpRequest::getMethod(void) const { return method_; }
const std::string&				HttpRequest::getPath(void) const { return path_; }
const std::map<std::string, std::string>&	HttpRequest::getHeader(void) const { return header_; }
const std::vector<char>&	HttpRequest::getBody(void) const { return body_; }
const int&								HttpRequest::getContentLength(void) const { return content_length_; }
const int&								HttpRequest::getPort(void) const { return port_; }
const bool&								HttpRequest::getIsChunked(void) const { return is_chunked_; }
const bool&								HttpRequest::getExistSession(void) const { return exist_session_; }
const std::string&				HttpRequest::getContentType(void) const { return content_type_; }
const std::string&				HttpRequest::getHost(void) const { return host_; }
const e_requestError&			HttpRequest::getRequestError(void) const { return request_error_; }
const e_parseStatus&			HttpRequest::getParseStatus(void) const { return parse_status_; }
void											HttpRequest::setPort(int port) { port_ = port; }

void HttpRequest::printRequestInfo(){
	std::cout << "-----------------PARSE RESULT------------------" << std::endl;
	std::cout << "method: " << method_ << std::endl;
	std::cout << "path: " << path_ << std::endl;
	std::cout << "port: " << port_ << std::endl;
	std::cout << "is_chunked_: " << static_cast<int>(is_chunked_) << std::endl;
	std::cout << "content_length_: " << content_length_ << std::endl;
	std::cout << "content_type_: " << content_type_ << std::endl;
	std::cout << "host_: " << host_ << std::endl;
	std::cout << "parse_status_: " << static_cast<int>(parse_status_) << std::endl;
	std::cout << "request_error_: " << static_cast<int>(request_error_) << std::endl;
	std::cout << "exist_session_: " << static_cast<int>(exist_session_) << std::endl;
	for (std::map<std::string, std::string>::iterator i = header_.begin(); i != header_.end(); i++) {
		std::cout << "[header " << i->first << "] " << i->second << std::endl;
	}
	std::cout << "-----------------BODY------------------" << std::endl;
	// print_vec(body_);
	std::cout << "-----------------------------------------------" << std::endl;

}

/**
 * @brief HttpRequest를 파싱하는 함수입니다.
 * request_error_가 발생하거나 parse_status_가 FINISH가 되기 전까지
 * raw_data에 있는 값을 가지고 최대한 파싱을 진행합니다.
 * 
 * @param raw_data 
 * @note request_error_가 발생하면 parse_status_는 FINISH가 됩니다.
 */
void HttpRequest::parse(std::vector<char>& raw_data) {
	while (true) {
		if (request_error_) {
			parse_status_ = FINISH;
			return ;
		}
		switch (parse_status_) {
			case FINISH:
				return ;

			case BODY:
				if (!parseBody(raw_data))
					return;
				break;

			case HEADER:
				if (!hasCRLF(raw_data)) return ;
				parseHeader(getLine(raw_data));
				break;

			case FIRST:
				if (!hasCRLF(raw_data)) return ;
				parseFirstLine(getLine(raw_data));
				break;
		}
	}
}

/**
 * @brief HTTP 메세지의 첫 번째 라인을 해석하는 함수입니다.
 *
 * @param line string reference로, 함수 내부에서 변경됩니다.
 * @note *FORM_ERROR, METHOD_ERROR, VERSION_ERROR* 가 발생할 수 있습니다.
 * @note 정상 실행 후 parse_status_가 HEADER로 변경됩니다.
 */
void	HttpRequest::parseFirstLine(std::string line) {
	const char* method_tmp[] = {"GET", "HEAD", "DELETE", "POST", "PUT", "PATCH"};
	std::vector<const char*> methods(method_tmp, method_tmp + 6);
	std::string	target;
	int					method;
	double			version;

	/* method 분리 */
	target = getTarget(line);
	if (request_error_) return ;
	method = find(methods.begin(), methods.end(), target) - methods.begin();
	if (method < 0 || method >= 6) {
		request_error_ = METHOD_ERROR;
		return ;
	}
  method_ = static_cast<e_method>(method);

	/* path 분리 */
	target = getTarget(line);
	if (request_error_) return ;
  path_ = target;

	/* version 확인 */
	if (line.find("HTTP/", 0, 5) == std::string::npos) {
		request_error_ = FORM_ERROR;
		return ;
	}
	std::stringstream ss(line.substr(5));
	ss >> version;
	if (ss.tellg() != -1) {
		request_error_ = FORM_ERROR;
		return ;
	}
	if (version < 1.1) {
		request_error_ = VERSION_ERROR;
		return ;
	}
	
	parse_status_ = HEADER;
}

/**
 * @brief HTTP 메세지의 Header 한 줄을 해석하여 key, value를 lower-case로 저장하는 함수입니다.
 *
 * @param line
 * @note 파싱할 때 *FORM_ERROR* 가 발생할 수 있습니다.
 * @note 빈 문자열이 들어올 시 parse_status_를 BODY로 변경 및 필요한 헤더 존재여부 확인 후 종료합니다.
 * @note 헤더 확인 과정에서 *FORM_ERROR, UNIMPLEMENTED_ERROR, LENGTH_REQUIRED_ERROR*가 발생할 수 있습니다.
 */
void	HttpRequest::parseHeader(std::string line) {
	std::string	key, value;
	size_t			split_idx;

	/* header 끝 */
	if (line == "") {
		checkHeader();
		if (is_chunked_ || content_length_ != -1)
			parse_status_ = BODY;
		else
			parse_status_ = FINISH;
		return ;
	}

	/* header 추가 */
	/* 이전 헤더에 연결되는 값 */
	if (line[0] == ' ' || line[0] == '\t') {
		trimSidesSpace(line);
		lowerString(line);
		if (last_header_ == "") {
			request_error_ = FORM_ERROR;
			return ;
		}
		header_[last_header_] += line;
		return ;
	}

	/* 새로운 헤더 */
	split_idx = line.find(':');
	if (split_idx == std::string::npos) {
		request_error_ = FORM_ERROR;
    return ;
	}
	key = line.substr(0, split_idx);
	value = line.substr(split_idx + 1);
	if (key == "") {
		request_error_ = FORM_ERROR;
    return ;
	}
	lowerString(key);
	trimSidesSpace(value);

	last_header_ = key;
	if (key == "cookie") {
		if (header_.find(key) != header_.end()) {
			std::vector<std::string> list = split(header_[key], "; ");
			if (std::find(list.begin(), list.end(), value) == list.end())
				header_[key] += "; " + value;
		}
		else
			header_[key] = value;
		return ;
	}

	lowerString(value);
	if (header_.find(key) == header_.end())
		header_[key] = value;
	else if (get_multiple_header().find(key) != get_multiple_header().end() && !get_multiple_header().at(key))
		header_[key] = value;
	else {
		std::vector<std::string> list = split(header_[key], ", ");
		if (std::find(list.begin(), list.end(), value) == list.end())
			header_[key] += ", " + value;
	}
}

/**
 * @brief content-type, transfer-encoding, content-length 확인하여 변수에 저장하는 함수입니다.
 *
 * @note transfer-encoding에 chunked만 있지 않을 때 *UNIMPLEMENTED_ERROR*가 발생합니다.
 * @note content-length가 올바른 숫자가 아니거나 0보다 작을 때 *FORM_ERROR*가 발생합니다.
 * @note transfer-encoding과 content-length 둘 다 존재하지 않으면 *LENGTH_REQUIRED_ERROR*가 발생합니다.
 */
void	HttpRequest::checkHeader(void) {
	/* host */
	if (header_.find(std::string("host")) != header_.end())
		host_ = header_["host"];

	/* cookie */
	if (header_.find(std::string("cookie")) != header_.end()) {
		std::vector<std::string> list = split(header_["cookie"], "; ");
		size_t	pos;

		for (std::vector<std::string>::const_iterator it = list.begin(); it != list.end(); it++) {
			pos = (*it).find("=");
			if (isExist(pos)) {
				cookie_[(*it).substr(0, pos)] = (*it).substr(pos + 1);
			} else {
				request_error_ = FORM_ERROR;
				return ;
			}
		}
		if (cookie_.find("SESSIONID") != header_.end() && \
			Session::getInstance().existSession(cookie_["SESSIONID"]))
			exist_session_ = true;
	}

	/* content-type */
	if (header_.find(std::string("content-type")) != header_.end())
		content_type_ = header_["content-type"];


	/* transfer-encoding */
	if (header_.find(std::string("transfer-encoding")) != header_.end()) {
		std::vector<std::string> list = split(header_["transfer-encoding"], ", ");
		if (list.size() != 1 || list[0] != "chunked")
			request_error_ = UNIMPLEMENTED_ERROR;
		else
			is_chunked_ = true;
		return ;
	}

	/* content-length */
	if (header_.find(std::string("content-length")) != header_.end()) {
		std::stringstream ss(header_["content-length"]);
		ssize_t length;

		ss >> length;
		if (length < 0 || ss.tellg() != -1) {
			request_error_ = FORM_ERROR;
			return ;
		}
		content_length_ = length;
	} else if (method_ == POST || method_ == PUT) {
		request_error_ = LENGTH_REQUIRED_ERROR;
	}
}

/**
 * @brief Body를 받는 함수
 * [chunked가 아닌 경우]
 * raw_data_는 content_length만큼 받지 않았으면 raw_data_를 비우지 않고 false를 반환한다.
 * [chunked인 경우]
 * 1. CRLF를 찾아서 raw_data를 읽은 만큼 비워줍니다.
 * 2. 이후 16진수를 해석해서 그만큼 또 raw_data_를 비워줍니다.
 * 3. 0이 나올 때까지 반복합니다.
 * @param raw_data 들어온 데이터를 저장하는 저장소
 * @return true 파싱 계속 진행 가능(에러 포함: 다음 반복문에서 끝남)
 * @return false 버퍼를 계속 받아야 진행 가능
 * @warning body 파싱 중 에러가 나도 FORM ERROR로 처리하고 return true 합니다.
 */
bool	HttpRequest::parseBody(std::vector<char>& raw_data){
	if (is_chunked_)
		return parseChunkedBody(raw_data);
	else if ((size_t)content_length_ <= raw_data.size()){
		body_.reserve(body_.size() + content_length_);
		body_.insert(body_.end(), raw_data.begin(), raw_data.begin() + content_length_);
		raw_data.erase(raw_data.begin(), raw_data.begin() + content_length_);
		parse_status_ = FINISH;
		return true;
	}
	return false;
}

/**
 * @brief Chunked Body를 받는 함수
 * 1. CRLF를 찾아서 raw_data를 읽은 만큼 비워줍니다.
 * 2. 이후 16진수를 해석해서 그만큼 또 raw_data_를 비워줍니다.
 * 3. 0이 나올 때까지 반복합니다.
 * 
 * @param raw_data 
 * @return true 파싱 계속 진행 가능(에러 포함: 다음 반복문에서 끝남)
 * @return false 버퍼를 계속 받아야 진행 가능
 */
bool HttpRequest::parseChunkedBody(std::vector<char>& raw_data) {
	/* to_read_ 받아야 함 */
	if (!read_state_) {
		if (hasCRLF(raw_data) == false)
			return false;

		std::string ret = getLine(raw_data);
		to_read_ = hexToDec(ret);
		if (to_read_ == -1) {
			request_error_ = FORM_ERROR;
			return true;
		}
		read_state_ = true;
	}

	/* to_read_ 확정된 후 */
	if (to_read_ == 0 && raw_data.size() >= 2) {
		if (getLine(raw_data) != "")
			request_error_ = FORM_ERROR;

		read_state_ = false;
		parse_status_ = FINISH;
		return true;
	} else if (raw_data.size() >= (size_t)to_read_ + 2) { // CRLF 보장을 위해 + 2
		raw_data.reserve(raw_data.size() + to_read_);
		body_.insert(body_.end(), raw_data.begin(), raw_data.begin() + to_read_);
		raw_data.erase(raw_data.begin(), raw_data.begin() + to_read_);
		to_read_ = 0;
		read_state_ = false;

		if (getLine(raw_data) != "")
			request_error_ = FORM_ERROR;
		return true;
	}
	return false;
}

/**
 * @brief findCRLF()를 사용하여 CRLF을 기준으로 문자열을 반환한 뒤, raw_data에서 해당 부분을 삭제합니다.
 *
 * @attention *raw_data에 CRLF가 존재할 때에만* 이 함수를 사용해야 합니다. 함수 실행 이전에 hasCRLF()로 확인을 요합니다.
 * @param raw_data vector<char> reference로, 함수 내부에서 변경됩니다.
 * @return std::string
 * @note raw_data에 CRLF가 존재하지 않을 때, *FORM_ERROR* 가 발생할 수 있습니다.
 */
std::string HttpRequest::getLine(std::vector<char>& raw_data) {
  std::string line;
  size_t      split_idx;

  if (!hasCRLF(raw_data)) {
		request_error_ = FORM_ERROR;
    return "";
  }
  split_idx = findCRLF(raw_data);
	line = std::string(&raw_data[0], split_idx);
	raw_data.erase(raw_data.begin(), raw_data.begin() + split_idx + 2);
  return line;
}

/**
 * @brief line에서 공백을 기준으로 string을 분리한 뒤, 분리한 부분은 기존 line에서 삭제합니다.
 *
 * @param line string reference로, 함수 내부에서 변경됩니다.
 * @return std::string 공백이 존재하지 않을 시 빈 문자열을 반환합니다.
 * @note *FORM_ERROR* 가 발생할 수 있습니다.
 */
std::string	HttpRequest::getTarget(std::string& line) {
	std::string	target = "";
	size_t			split_idx;

	split_idx = line.find(' ');
	if (split_idx == std::string::npos) {
		request_error_ = FORM_ERROR;
		return target;
	}
	target = line.substr(0, split_idx);
	line.erase(line.begin(), line.begin() + split_idx + 1);
	return target;
}

/**
 * @brief rfc 2616에 존재하는 모든 헤더에 대하여 multiple value를 가지는 지 여부를 나타내는 map을 반환하는 함수
 *
 * @note HttpRequest 클래스의 static 함수입니다.
 * @return const std::map<std::string, bool>
 */
const std::map<std::string, bool> HttpRequest::get_multiple_header() {
	std::map<std::string, bool> map;

	map["accept"] = true;
	map["accept-charset"] = true;
	map["accept-encoding"] = true;
	map["accept-language"] = true;
	map["accept-ranges"] = false;
	map["age"] = false;
	map["allow"] = true;
	map["authorization"] = false;
	map["cache-control"] = true;
	map["connection"] = true;
	map["content-encoding"] = true;
	map["content-language"] = true;
	map["content-length"] = false;
	map["content-location"] = false;
	map["content-md5"] = false;
	map["content-range"] = false;
	map["content-type"] = false;
	map["cookie"] = true;
	map["date"] = false;
	map["etag"] = false;
	map["expect"] = true;
	map["expires"] = false;
	map["from"] = false;
	map["host"] = false;
	map["if-match"] = true;
	map["if-modified-since"] = false;
	map["if-none-match"] = true;
	map["if-range"] = false;
	map["if-unmodified-since"] = false;
	map["last-modified"] = false;
	map["location"] = false;
	map["max-forwards"] = false;
	map["pragma"] = true;
	map["proxy-authenticate"] = true;
	map["proxy-authorization"] = false;
	map["range"] = false;
	map["referer"] = false;
	map["retry-after"] = false;
	map["server"] = false;
	map["te"] = true;
	map["trailer"] = true;
	map["transfer-encoding"] = true;
	map["upgrade"] = true;
	map["user-agent"] = false;
	map["vary"] = true;
	map["via"] = true;
	map["warning"] = true;
	map["www-authenticate"] = true;

	return map;
}

/**
 * @brief 16진수 string을 int로 변환하는 함수입니다.
 *
 * @param base_num 16진수 숫자 string입니다.
 * @return int 변경완료한 함수
 * @warning
 * 1. 숫자가 아닌경우
 * 2. 음수인 경우
 * 3. overflow
 * 전부 e_parseError	parse_error_ 변수를 FORM_ERROR로 초기화합니다.
 */
int	HttpRequest::hexToDec(const std::string& base_num){
	double ret = 0;
	for (std::string::const_iterator it = base_num.begin(); it != base_num.end(); it++){
		ret *= 16;
		if (*it >= '0' && *it <= '9'){
			ret += *it -'0';
		}else if ('A' <=*it && *it <='F'){
			ret+= *it-'A' + 10;
		}else if ('a' <= *it && *it <= 'f'){
				ret+= *it-'a' + 10;
		}
		else{
			request_error_ = FORM_ERROR;
			return -1;
		}
	}
	int a = ret;
  if (static_cast<double> (a) != std::floor(ret)){
		request_error_ = FORM_ERROR;
		return (-1);
	}
	return (a);
}
