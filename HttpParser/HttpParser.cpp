#include "HttpParser.hpp"

/**
 * 파싱 순서 정리 **일단 헤더까지만**
 * 1. raw_data_ 에 buff 이어 붙이기
 * 2. client의 http_request의 파싱이 완료되었고, 비어있지 않다면? //* parseStatus == FIN
 * 2-1. 요청 파싱 끝
 * 3. raw_data_에서 CRLF 기준으로 앞부분 떼어 오고 기존 raw_data_에서는 삭제
 * * 시작 줄 -> 헤더 -> 본문 순으로 채운다.: 현재 단계 확인 필요
 * 4. 시작 줄 비어있을 때 파싱: '메서드 URL 버전' 순으로 하나의 공백을 가지고 분리하여 저장한다.
 * 4-1. 메서드가 우리가 지원하는 것이 아니라면 501 or 405 error
 * * 405 Method Not Allowed vs 501 Not Implemented: 405는 구현되어있으나 deny, 501은 구현이 안되어있음. 난데없는 메소드라면 400번대로 하는 게 맞는데, 표준에 있는데 구현하지 않는 것도 있어서 500번대도 나쁘지 않다고 생각 ... 애매
 * TODO: 4-1 에러 정하기
 * 4-2. 버전이 우리가 지원하는 것이 아니라면 505 error
 * 5. 빈 줄 들어오기 전까지 헤더에 집어넣기: 3번에서 CRLF 기준으로 다 잘라옴.
 * 5-1. 헤더의 값이 비어 있으면 406 Not Accepted or 그냥 빈 대로 두기
 * TODO: 5-1 확정하기
 * TODO: 바디 없는 요청의 경우, 헤더 마지막에 CRLF가 두번 나오지 않을 수 있음. -> 처리할 것인가?
 * * 처리한다면, 파싱을 했을 때 원하는 꼴이 나오지 않고 시작 줄 양식에는 맞다면 분리하는 방식으로 갈 것 같은데 이거 너무 오바하는 것 아닌가 싶고 ..
 */

// TODO: UData에 fd 추가?

void HttpParser::parse(char* buff, size_t len, UData& u_data) const {
	std::vector<char>&	raw_data = u_data.raw_data_; // TODO: 제대로 되는 지 확인 필요
	std::string					line;
	size_t							split_idx;

	raw_data.insert(raw_data.end(), buff, buff + len);
	// if (parseStatus == FIN), parse fin.
	// if (parseStatus == BODY), use body parsing func.
	
	/* BODY 아님을 가정하여 진행 */
	split_idx = findCRLF(raw_data);
	if (split_idx == raw_data.size())	return ; // there isn't CRLF in raw data.
	line = std::string(&raw_data[0], split_idx);
	raw_data.erase(raw_data.begin(), raw_data.begin() + split_idx + 2);

	// if (parseStatus == FIRST)
	parseFirstLine(line, u_data);
	// TODO: 에러 확인
}

size_t HttpParser::findCRLF(const std::vector<char>& raw_data) const {
	for (size_t i = 0; i < raw_data.size() - 1; i++) {
		if (raw_data[i] == '\r' && raw_data[i + 1] == '\n')
			return i;
	}
	return raw_data.size();
}

std::string& HttpParser::getTarget(std::string& line) const {
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

void	HttpParser::parseFirstLine(std::string& line, UData& u_data) const {
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
	u_data.http_request_.setMethod(static_cast<e_method>(method));

	/* path 분리 */
	target = getTarget(line);
	u_data.http_request_.setPath(target);

	/* version */
	if (line != "HTTP/1.1") {
		// wrong version error
	}
}

bool	HttpParser::parseHeader(std::string& line, UData& u_data) const {
	std::string	key, value;
	size_t			split_idx;

	split_idx = line.find(':');
	if (split_idx == std::string::npos) {
		// wrong form error
		return false;
	}
	key = line.substr(0, split_idx);
	value = line.substr(split_idx + 1);
	// insert header
	return true;
}
