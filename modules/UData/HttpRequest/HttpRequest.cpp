#include "../UData.hpp"

/**
 * 파싱 순서 정리 (new) *HttpRequest가 vector<HttpRequest>의 형태로 변함에 따라 파싱 알고리즘이 변화되었습니다.*
 * ---- event handler ----
 * 1. event를 handle하는 곳에서 raw_data에 buff를 더합니다.
 * 2. request가 없거나, 모든 request가 FINISH 상태일 때 새 HttpRequest를 생성합니다.
 * 3. FIN이 아닌 HttpRequest에 대해 raw_data를 인자로 parse 함수를 실행합니다.
 * ---- parse 함수 실행 ----
 * 주어진 raw_data에 대해 가능한 모든 파싱을 진행합니다.
 * 각 request마다 오류가 발생했을 시 flag를 설정합니다
 * * FORM_ERROR: 400 Bad Request
 * * METHOD_ERROR: 405 Method Not Allowed -> ** TODO: response에 반드시 Allow 헤더가 포함되어야 합니다. **
 * * VERSION_ERROR: 505 HTTP Version Not Supported
 * ---- parse 함수 끝 ----
 * event handler는 해당 HttpRequest가 FIN이고, raw_data가 비어있지 않을 시 다음 HttpRequest 파싱을 진행합니다.
 * 
 * TODO: 오류 발생 시 플로우 생각해보기
 * TODO: 헤더에 여러줄 올 수도 있넴요 . .
 * 
 * * 바디 없는 요청의 경우, 헤더 마지막에 CRLF가 두번 나오지 않을 수 있음. (일단 처리 x)
 * * 처리한다면, 파싱을 했을 때 원하는 꼴이 나오지 않고 시작 줄 양식에는 맞다면 분리하는 방식으로 ..
 */

HttpRequest::HttpRequest(): parse_status_(FIRST), parse_error_(OK) { }

void HttpRequest::parse(std::vector<char>& raw_data) {
	while (true) {
		if (parse_error_)
			return ;
		switch (parse_status_) {
			case FINISH:
				return ;
			case BODY:
				/**
				 * body parsing 진행: 리턴 값은 완료 시 true, 아직 끝나지 않았을 때 false
				 * true일 때 status를 FIN으로 바꿔준다.
				 * false일 때 함수를 종료한다.
				 */
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
	// size_t	split_idx;
	int			method;

	/* method 분리 */
	target = getTarget(line);
	if (parse_error_) return ;
	method = find(methods.begin(), methods.end(), target) - methods.begin();
	if (method < 0 || method >= 6) {
		parse_error_ = METHOD_ERROR;
		return ;
	}
  method_ = static_cast<e_method>(method);

	/* path 분리 */
	target = getTarget(line);
	if (parse_error_) return ;
  path_ = target;

	/* version */
	if (line != "HTTP/1.1") {
		parse_error_ = VERSION_ERROR;
		return ;
	}
	parse_status_ = HEADER;
}

/**
 * @brief HTTP 메세지의 Header 한 줄을 해석하는 함수입니다.
 * 
 * @param line string reference로, 함수 내부에서 변경됩니다.
 * @note *FORM_ERROR* 가 발생할 수 있습니다.
 * @note 빈 문자열이 들어올 시 parse_status_를 BODY로 변경 및 종료합니다.
 */
void	HttpRequest::parseHeader(std::string line) {
	std::string	key, value;
	size_t			split_idx;

	if (line == "") {
		parse_status_ = BODY;
		return ;
	}
	split_idx = line.find(':');
	if (split_idx == std::string::npos) {
		parse_error_ = FORM_ERROR;
    return ;
	}
	key = line.substr(0, split_idx);
	value = line.substr(split_idx + 1);
	trimComment(value);
	header_.insert(std::pair<std::string, std::string>(lowerString(key), value));
}

void	HttpRequest::checkHeader(void) {
	// if (header_.find(std::string("transfer-encoding")) != header_.end()) {
	// 	if (header_["transfer-encoding"] == "")
	// }
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
		parse_error_ = FORM_ERROR;
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
		parse_error_ = FORM_ERROR;
		return target;
	}
	target = line.substr(0, split_idx);
	line.erase(line.begin(), line.begin() + split_idx + 1);
	return target;
}
