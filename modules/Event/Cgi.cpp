#include "Cgi.hpp"

Cgi::Cgi(){}

Cgi::~Cgi(){}

/**
 * @brief CGI 자식 프로세스에게 전달할 envp를 제공하는 함수입니다.
 * 메소드, 헤더, 바디가 순서대로 envp에 저장됩니다.
 * @param req CGI 프로세스를 생성할 HttpRequest 클래스 인스턴스 레퍼런스입니다.
 * @return 완성된 envp입니다.
 */
char**  Cgi::getEnvs(UData* ptr){
  HttpRequest&  req = ptr->http_request_[0];
  char** envp = new char*[19];
  envp[18] = NULL;
  envp[0] = (char *)"AUTH_TYPE=NULL";
  if (req.getMethod() == POST)
    envp[1] = (char *)std::string("CONTENT_LENGTH=" + std::to_string(req.getBody().size())).c_str(); //어차피 Body Length이므로
  else
    envp[1] = (char *)"CONTENT_LENGTH=-1";
  std::cerr << "CONTENT_TYPE: " << req.getContentType() << std::endl;
  envp[2] = (char *)std::string("CONTENT_TYPE=" + req.getContentType()).c_str();
  envp[3] = (char *)"GATEWAY_INTERFACE=CGI/1.1";
  // envp[4] = "PATH_INFO=/";
  // envp[5] = "PATH_TRANSLATED=/";
  // envp[6] = (char *)std::string("QUERY_STRING=" + req.getQueryString()).c_str();
  // envp[7] = (char *)std::string("REMOTE_ADDR=" + ptr->client_ip_).c_str();
  // envp[8] = "REMOTE_HOST=NULL";
  // envp[9] = "REMOTE_IDENT=NULL";
  // envp[10] = "REMOTE_USER=NULL";
  // envp[11] = (char *)std::string("REQUEST_METHOD=" + req.getMethodString()).c_str();
  // envp[12] = (char *)std::string("SCRIPT_NAME=" + ptr->prog_name_).c_str();
  // envp[13] = (char *)std::string("SERVER_NAME=" + ptr->server_name_).c_str();
  // envp[14] = (char *)std::string("SERVER_PORT=" + std::to_string(ptr->server_port_)).c_str();
  // envp[15] = (char *)std::string("SERVER_PROTOCOL=" + req.getVersion()).c_str();
  // envp[16] = "SERVER_SOFTWARE=webserv/1.0";
  return (envp);
}

/**
 * @brief 새로운 Cgi 프로세스를 생성하는 함수입니다.
 * 파이프 생성, 논블로킹 설정, UData 할당, 이벤트 등록 후 자식프로세스를 생성합니다.
 * 자식 프로세스는 주어진 Cgi 스크립트를 실행합니다.
 * @param req CGI 프로세스를 생성할 HttpRequest 클래스 인스턴스 레퍼런스입니다.
 * @param ptr 호출한 클라이언트가 사용하는 udata입니다. CGI타입으로 변경됩니다.
 * @exception 위 과정에서 에러 발생 시 runtime_error를 throw합니다.
 */
void  Cgi::forkCgi(UData* ptr){
  int   pfd[2];
  pid_t child_pid;

  if (pipe(pfd) == -1)
    throw (std::runtime_error("pipe() Error"));
  fcntl(pfd[0], F_SETFL, O_NONBLOCK);
  char* script_name;
  std::string cgi_path = ptr->http_response_.file_path_;
  ptr->prog_name_ = cgi_path;
  script_name = (char *)cgi_path.c_str();
  ptr->fd_type_ = CGI;
  Kqueue::registerReadEvent(pfd[0], ptr);
  Kqueue::unregisterReadEvent(ptr->client_fd_, ptr);
  child_pid = fork();
  if (child_pid == -1){
    close(pfd[1]);
    close(pfd[0]);
    throw (std::runtime_error("fork() Error"));
  }
  else if (!child_pid){ //child
    close(pfd[0]);
    dup2(pfd[1], STDOUT_FILENO);
    close(pfd[1]);
    int flags = fcntl(STDOUT_FILENO, F_GETFL, 0);
    fcntl(STDOUT_FILENO, F_SETFL, flags | O_NONBLOCK);
    char* exec_file[3];
    exec_file[0] = (char*)ptr->prog_name_.c_str();
    exec_file[1] = script_name;
    exec_file[2] = NULL;
    char** envp = getEnvs(ptr);
    // for (int i = 0; envp[i]; i++)
    //   std::cerr << "ENVP" << i << ": " << envp[i] << std::endl;
    std::cerr << "\nCGI1: " << exec_file[0];
    std::cerr << "\nCGI2: " << exec_file[1] << std::endl;
    if (execve(exec_file[0], exec_file, envp) == -1){//envp needed
      delete [] envp;
      throw (std::runtime_error("execve() Error"));
    }
  }
  //parent
  close(pfd[1]);
  Kqueue::registerExitEvent(child_pid, ptr); 
  ptr->cgi_pid_ = child_pid;
}
