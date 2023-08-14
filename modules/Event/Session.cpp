#include "Session.hpp"

Session& Session::getInstance() {
  static Session  session;
  return session;
}

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
		if (session_.find(random_str) == session_.end()) {
			session_[random_str] = "user" + intToString(session_.size());
			break ;
		}
	}
	return random_str;
}

bool  Session::existSession(std::string id) {
  return session_.find(id) != session_.end();
}
