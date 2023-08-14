#include "Session.hpp"

/**
 * @brief 싱글톤 패턴을 위해 static 변수를 반환합니다.
 * 
 * @return Session& 
 */
Session& Session::getInstance() {
  static Session  session;
  return session;
}

/**
 * @brief 랜덤 문자열을 만들어 세션을 만든 뒤 해당 문자열을 반환합니다.
 * 
 * @return std::string 
 */
std::string	Session::createSession(void) {
	const std::string characters = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
  const int					length = 16;
  std::string				random_str;

  srand(static_cast<unsigned int>(time(NULL)));
	while (true) {
		/* 랜덤 문자열 생성 */
		random_str = "";
	  for (int i = 0; i < length; ++i)
	      random_str += characters[rand() % characters.length()];
		
		/* 존재하는 문자열인 지 확인 */
		if (!existSession(random_str)) {
			session_[random_str] = "user" + intToString(session_.size());
			break ;
		}
	}
	return random_str;
}

/**
 * @brief 해당 id가 존재하는 세션을 가리키는 지 여부를 반환합니다.
 * 
 * @param id 
 * @return true 
 * @return false 
 */
bool  Session::existSession(std::string id) {
  return session_.find(id) != session_.end();
}
