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

void HttpParser::parse(char* buff, size_t len, UData u_data) {
	u_data.raw_data_.insert(u_data.raw_data_.end(), buff, buff + len);
	// if (parseStatus == FIN), parse fin.

}


