#include "../interface/UData.hpp"

/**
 * 파싱 순서 정리 **일단 헤더까지만**
 * 1. raw_data_ 에 buff 이어 붙이기
 * 2. client의 http_request의 파싱이 완료되었고, 비어있지 않다면? //* parseStatus == FIN
 * 2-1. 요청 파싱 끝
 * 3. raw_data_에서 CRLF 기준으로 앞부분 떼어 오고 기존 raw_data_에서는 삭제
 * * 시작 줄 -> 헤더 -> 본문 순으로 채운다.: 현재 단계 확인 필요
 * 4. 시작 줄 비어있을 때 파싱: '메서드 URL 버전' 순으로 하나의 공백을 가지고 분리하여 저장한다.
 * 4-1. 메서드가 우리가 지원하는 것이 아니라면 405 Method Not Allowed
 * 4-2. 버전이 우리가 지원하는 것이 아니라면 505 error
 * 5. 빈 줄 들어오기 전까지 헤더에 집어넣기: 3번에서 CRLF 기준으로 다 잘라옴.
 * 5-1. 헤더의 값이 비어 있으면 그냥 빈 대로 두기
 * * 바디 없는 요청의 경우, 헤더 마지막에 CRLF가 두번 나오지 않을 수 있음. (일단 처리 x)
 * * 처리한다면, 파싱을 했을 때 원하는 꼴이 나오지 않고 시작 줄 양식에는 맞다면 분리하는 방식으로 ..
 */

e_error HttpRequest::parse(char* buff, size_t len, std::vector<char>& raw_data) {
	std::string					line;
	size_t							split_idx;

	raw_data.insert(raw_data.end(), buff, buff + len);
	// if (parseStatus == FIN), parse fin. //!FIN
	// if (parseStatus == BODY), use body parsing func. //!FIN
	line = getLine(raw_data);

	// if (parseStatus == FIRST)
	parseFirstLine(line);
	// TODO: 에러 확인
}

/**
 * @brief HTTP 메세지의 첫 번째 라인을 해석하는 함수입니다.
 * 
 * @param line string reference로, 함수 내부에서 변경됩니다.
 */
void	HttpRequest::parseFirstLine(std::string& line) {
	const std::vector<std::string>	methods = {"GET", "HEAD", "DELETE", "POST", "PUT", "PATCH"};
	std::string	target;
	size_t	split_idx;
	int			method;

	/* method 분리 */
	target = getTarget(line);
	method = find(methods.begin(), methods.end(), target) - methods.begin();
	if (method < 0 || method >= 6) {
		// wrong method error
	}
  method_ = static_cast<e_method>(method);

	/* path 분리 */
	target = getTarget(line);
  path_ = target;

	/* version */
	if (line != "HTTP/1.1") {
		// wrong version error
	}
}

/**
 * @brief HTTP 메세지의 Header 한 줄을 해석하는 함수입니다.
 * 
 * @param line string reference로, 함수 내부에서 변경됩니다.
 */
void	HttpRequest::parseHeader(std::string& line) {
	std::string	key, value;
	size_t			split_idx;

	split_idx = line.find(':');
	if (split_idx == std::string::npos) {
		// wrong form error
    return ;
	}
	key = line.substr(0, split_idx);
	value = line.substr(split_idx + 1);
	header_.insert(std::pair<std::string, std::string>(key, value));
}

/**
 * @brief vector<char>에서 CRLF의 위치를 찾습니다.
 * 
 * @param raw_data 
 * @return size_t CRLF이 시작하는 index를 반환합니다.
 * @warning CRLF가 존재하지 않을 때 vector<char>의 size를 반환합니다.
 */
size_t HttpRequest::findCRLF(const std::vector<char>& raw_data) const {
	for (size_t i = 0; i < raw_data.size() - 1; i++) {
		if (raw_data[i] == '\r' && raw_data[i + 1] == '\n')
			return i;
	}
	return raw_data.size();
}

/**
 * @brief findCRLF()를 이용하여 CRLF의 존재 여부를 확인합니다.
 * 
 * @param raw_data 
 * @return true : CRLF 존재함
 * @return false : CRLF 존재하지 않음
 */
bool HttpRequest::hasCRLF(const std::vector<char>& raw_data) const {
  size_t  split_idx;

  split_idx = findCRLF(raw_data);
	if (split_idx == raw_data.size())
    return false;
  return true;
}

/**
 * @brief findCRLF()를 사용하여 CRLF을 기준으로 문자열을 반환한 뒤, raw_data에서 해당 부분을 삭제합니다.
 * 
 * @attention *raw_data에 CRLF가 존재할 때에만* 이 함수를 사용해야 합니다. 함수 실행 이전에 hasCRLF()로 확인을 요합니다.
 * @param raw_data vector<char> reference로, 함수 내부에서 변경됩니다.
 * @return std::string& 
 */
std::string& HttpRequest::getLine(std::vector<char>& raw_data) const {
  std::string line;
  size_t      split_idx;

  if (hasCRLF(raw_data)) {
    // wrong form error
    return ;
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
 * @return std::string& 공백이 존재하지 않을 시 빈 문자열을 반환합니다.
 * @note *FORM_ERROR* 가 발생할 수 있습니다.
 */
std::string& HttpRequest::getTarget(std::string& line) const {
	std::string	target = "";
	size_t			split_idx;

	split_idx = line.find(' ');
	if (split_idx == std::string::npos) {
		// wrong form error
		return target;
	}
	target = line.substr(0, split_idx);
	line.erase(line.begin(), line.begin() + split_idx + 1);
	return target;
}
