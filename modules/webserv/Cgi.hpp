#ifndef CGI_HPP
# define CGI_HPP
# include "../UData/HttpRequest/HttpRequest.hpp"
# include "../UData/UData.hpp"
# include "Kqueue.hpp"
# include <iostream>
# include <unistd.h>
# include <fcntl.h>

// - CGI Request form -
// POST /cgi-bin/simple_cgi.cgi HTTP/1.1
// Host: www.example.com
// User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64; rv:90.0) Gecko/20100101 Firefox/90.0
// Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,*/*;q=0.8
// Content-Type: application/x-www-form-urlencoded
// Content-Length: 23
// name=John&age=30

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