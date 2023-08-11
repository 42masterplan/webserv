#include "Cgi.hpp"

Cgi::Cgi(){}

Cgi::~Cgi(){}

/**
 * @brief CGI 자식 프로세스에게 전달할 envp를 제공하는 함수입니다.
 * 메소드, 헤더, 바디가 순서대로 envp에 저장됩니다.
 * @param req CGI 프로세스를 생성할 HttpRequest 클래스 인스턴스 레퍼런스입니다.
 * @return 완성된 envp입니다.
 */
char**  Cgi::getEnvs(HttpRequest& req){
  const std::map<std::string, std::string>&  header = req.getHeader();
  char**  envp = new char* [header.size() + 3];
  envp[header.size() + 2] = NULL;

  if (req.getMethod() == GET)
    envp[0] = (char *)"METHOD=GET";
  else if (req.getMethod() == POST)
    envp[0] = (char *)"METHOD=POST";
  std::string header_buff = "";
  std::map<std::string, std::string>::const_iterator it;
  int i = 1;
  for (it = header.begin(); it != header.end(); ++it, ++i) {
    header_buff = it->first + "=" + it->second;
    envp[i] = new char[header_buff.size() + 1];
    std::strcpy(envp[i], header_buff.c_str());
  }
  const std::vector<char>&  body = req.getBody();
  std::string body_buff = std::string(body.begin(), body.end());
  body_buff = "BODY=" + body_buff;
  envp[i] = new char[body_buff.size() + 1];
  std::strncpy(envp[i], body_buff.c_str(), body_buff.size() + 1);
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
void  Cgi::forkCgi(HttpRequest& req, UData* ptr){
  int   pfd[2];
  pid_t child_pid;

  if (pipe(pfd) == -1)
    throw (std::runtime_error("pipe() Error"));
  int flags = fcntl(pfd[0], F_GETFL, 0);
  fcntl(pfd[0], F_SETFL, flags | O_NONBLOCK);
  char* script_name;
  if (req.getPath().find(".php")){
    ptr->prog_name_ = "php";
    script_name = (char *)"./srcs/CGI_1.php";
  }
  else if (req.getPath().find(".py")){
    ptr->prog_name_ = "python3";
    script_name = (char *)"./srcs/CGI_2.py";
  }
  else
    throw std::runtime_error("invalid CGI path");
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
    int flags = fcntl(STDOUT_FILENO, F_GETFL, 0);
    fcntl(STDOUT_FILENO, F_SETFL, flags | O_NONBLOCK);
    char* exec_file[3];
    exec_file[0] = (char*)ptr->prog_name_.c_str();
    exec_file[1] = script_name;
    exec_file[2] = NULL;
    char** envp = getEnvs(req);
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
