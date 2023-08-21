#ifndef CGI_HPP
# define CGI_HPP
# include "../../UData/HttpRequest/HttpRequest.hpp"
# include "../Handler/HttpResponseHandler.hpp"
# include "../../UData/UData.hpp"
# include "Kqueue.hpp"
# include "../../Tools/ParseTool.hpp"

static const char* basic_env[] = {
  "AUTH_TYPE",
  "CONTENT_LENGTH",
  "CONTENT_TYPE",
  "GATEWAY_INTERFACE",
  "PATH_INFO",
  "PATH_TRANSLATED",
  "QUERY_STRING",
  "REMOTE_ADDR",
  "REMOTE_HOST",
  "REMOTE_IDENT",
  "REMOTE_USER",
  "REQUEST_METHOD",
  "SCRIPT_NAME",
  "SERVER_NAME",
  "SERVER_PORT",
  "SERVER_PROTOCOL",
  "SERVER_SOFTWARE",
  NULL};

/**
 * @brief CGI 프로세스를 생성하는 기능을 제공하는 정적 클래스입니다.
 */
class Cgi{
  public:
    static void   forkCgi(UData* ptr);
  private:
    Cgi();
    ~Cgi();
    static char** getEnvs(UData* ptr, std::vector<int>& del_vec);
};

#endif
