#ifndef CGI_HPP
# define CGI_HPP
# include <unistd.h>
# include <stdexcept>
# include <string>
# include "Kqueue.hpp"

/**
 * @brief 단일 CGI 프로세스를 구성할 클래스입니다.
 */
class CGI{
  public:
          CGI();
          ~CGI();
    void  forkCGI(std::string script_name, Kqueue* kq_ptr, int clnt_ident);

  private:
    int         cgi_pipe_[2];
    pid_t       cgi_pid_;
    std::string script_name_; // TODO: 실행시킬 파이썬 스크립트 이름 -> conf파일 CGI Path 질의
};

#endif