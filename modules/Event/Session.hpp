#ifndef SESSION_HPP
# define SESSION_HPP
# include <string>
# include <map>
# include "../Tools/ParseTool.hpp"

class Session {
  public:
    ~Session();
    static Session& getInstance();
		std::string     createSession(void);
    bool            existSession(std::string id);

  private:
    Session();
    std::map<std::string, std::string>	session_;
};

#endif
