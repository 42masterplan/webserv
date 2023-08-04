#ifndef CGI_HPP
# define CGI_HPP
# include "../UData/HttpRequest/HttpRequest.hpp"
# include "../UData/UData.hpp"
# include "Kqueue.hpp"
# include <iostream>
# include <unistd.h>
# include <fcntl.h>

/**
 * @brief CGI 프로세스를 생성하는 기능을 제공하는 정적 클래스입니다.
 */
class Cgi{
  public:
    static void   forkCgi(HttpRequest& req);
  private:
    Cgi();
    ~Cgi();
    static char** getEnvs(HttpRequest& req);
};

#endif