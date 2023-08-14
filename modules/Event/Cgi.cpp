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
  // const std::map<std::string, std::string>&  header = req.getHeader();
  char**  envp = new char* [4];
  envp[3] = NULL;

  if (req.getMethod() == GET)
    envp[0] = (char *)"REQUEST_METHOD=GET";
  else if (req.getMethod() == POST)
    envp[0] = (char *)"REQUEST_METHOD=POST";
  envp[1] = (char *)"SERVER_PROTOCOL=HTTP/1.1";
  std::string path_info = std::string("PATH_INFO=" + ptr->prog_name_);
  envp[2] = new char[path_info.size() + 1];
  std::strcpy(envp[2], path_info.c_str());
  //선택적으로 스크립트를 호출한 HTTP 요청의 추가 경로 정보를 포함하고 있으며 CGI 스크립트로 해석할 경로를 지정합니다.
  //PATH_INFO는 CGI 스크립트가 리턴할 자원 또는 하위 자원을 식별하며 스크립트 이름 뒤에 오지만 모든 조회 데이터 앞에 오는 URI 경로 부분에서 파생됩니다.
  // std::string header_buff = "";
  // std::map<std::string, std::string>::const_iterator it;
  // int i = 3;
  // for (it = header.begin(); it != header.end(); ++it, ++i) {
  //   header_buff = it->first + "=" + it->second;
  //   envp[i] = new char[header_buff.size() + 1];
  //   std::strcpy(envp[i], header_buff.c_str());
  //   std::cerr << "envp I: " << envp[i] << std::endl;
  // }
  // const std::vector<char>&  body = req.getBody();
  // std::string body_buff = std::string(body.begin(), body.end());
  // body_buff = "BODY=" + body_buff;
  // envp[i] = new char[body_buff.size() + 1];
  // std::strncpy(envp[i], body_buff.c_str(), body_buff.size() + 1);
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
    for (int i = 0; envp[i]; i++)
      std::cerr << "ENVP" << i << ": " << envp[i] << std::endl;
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
