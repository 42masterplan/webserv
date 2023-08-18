#include "Cgi.hpp"

Cgi::Cgi(){}

Cgi::~Cgi(){}

/**
 * @brief CGI 자식 프로세스에게 전달할 envp를 제공하는 함수입니다.
 * RFC3875기준의 CGI 환경변수를 사용했습니다.
 * @param ptr 사용할 UData 포인터입니다.
 * @return 완성된 envp입니다.
 */
char**  Cgi::getEnvs(UData* ptr){
  HttpRequest&  req = ptr->http_request_[0];
  char** envp = new char*[20];
  std::string buf;
  envp[18] = NULL;
  envp[19] = NULL;
  envp[0] = (char*)"AUTH_TYPE="; //서블릿이 보호되지 않는 경우 null입니다.
  if (req.getMethod() == POST){
    buf = std::string("CONTENT_LENGTH=" + std::to_string(req.getBody().size())); //content_length == Body Length이므로
    envp[1] = new char[buf.size() + 1];
    std::strcpy(envp[1], buf.c_str());
  }
  else
    envp[1] = (char*)"CONTENT_LENGTH=0";
  if (req.getContentType() == "")
    envp[2] = (char*)"CONTENT_TYPE=";
  else{
    buf = std::string("CONTENT_TYPE=" + req.getContentType()); //요청 본문의 MIME 유형 또는 유형을 모르는 경우 null
    envp[2] = new char[buf.size() + 1];
    std::strcpy(envp[2], buf.c_str());
  }
  envp[3] = (char*)"GATEWAY_INTERFACE=CGI/1.1"; //서버가 스크립트와 통신하기 위해 사용하는 CGI 스펙의 버전
  envp[4] = (char*)"PATH_INFO=/directory/youpi.bla"; //선택적으로 스크립트를 호출한 HTTP 요청의 추가 경로 정보
  buf = std::string("PATH_TRANSLATED=" + ptr->prog_name_); //루트 경로가 붙은 CGI 경로 ex.)/var/www/YoupiBanane/cgi_tester
  envp[5] = new char[buf.size() + 1];
  std::strcpy(envp[5], buf.c_str());
  envp[6] = (char*)"QUERY_STRING="; //쿼리 스트링 다루지 않음.
  envp[7] = (char*)"REMOTE_ADDR=127.0.0.1"; //웹 서버에 접속한 클라이언트의 IP 주소, 로컬 루프백 주소인 127.0.0.1
  envp[8] = (char*)"REMOTE_HOST=127.0.0.1"; //웹 서버에 접속한 클라이언트의 컴퓨터 이름이나 도메인 이름
  envp[9] = (char*)"REMOTE_IDENT="; //"Ident" 프로토콜에 의해 제공되는 사용자 신원 정보, Ident 프로토콜을 지원하고 활성화한 경우에만
  envp[10] = (char*)"REMOTE_USER="; //접속한 클라이언트의 인증된 사용자 이름, 인증 정보를 제공하고 인증이 성공한 경우에만 설정
  switch (req.getMethod()) {
    case POST:
      envp[11] = (char*)"REQUEST_METHOD=POST"; break;
		case GET: 
      envp[11] = (char*)"REQUEST_METHOD=GET"; break;
    default:
      envp[11] = (char*)"REQUEST_METHOD=INVALID";
  }
  envp[12] = (char*)"SCRIPT_NAME=/directory/youpi.bla";
  envp[13] = (char*)"SERVER_NAME=localhost"; //웹 서버의 호스트 이름 또는 도메인 이름
  buf = std::string("SERVER_PORT=" + std::to_string(ptr->port_)); //웹 서버가 사용하는 포트 번호
  envp[14] = new char[buf.size() + 1];
  std::strcpy(envp[14], buf.c_str());
  envp[15] = (char*)"SERVER_PROTOCOL=HTTP/1.1"; //HTTP/1.1 프로토콜을 사용
  envp[16] = (char*)"SERVER_SOFTWARE=webserv/1.0";
  envp[17] = (char*)"REQUEST_URI=/directory/youpi.bla";
  if (req.getHeader().find("x-secret-header-for-test") != req.getHeader().end()) {
    buf = std::string("HTTP_X_SECRET_HEADER_FOR_TEST=" + req.getHeader().at("x-secret-header-for-test"));
    envp[18] = new char[buf.size() + 1];
    std::strcpy(envp[18], buf.c_str());
  }

  return (envp);
}

/**
 * @brief 새로운 Cgi 프로세스를 생성하는 함수입니다.
 * 파이프 생성, 논블로킹 설정, UData 할당, 이벤트 등록 후 자식프로세스를 생성합니다.
 * 자식 프로세스는 주어진 Cgi 스크립트를 실행합니다.
 * @param ptr 호출한 클라이언트가 사용하는 udata입니다. CGI타입으로 변경됩니다.
 * @exception 위 과정에서 에러 발생 시 runtime_error를 throw합니다.
 */
void  Cgi::forkCgi(UData* ptr){
  int   del_arr[5] = {1, 2, 5, 14, 18};
  int   r_pfd[2];
  int   w_pfd[2];
  pid_t child_pid;

  if (pipe(r_pfd) == -1 || pipe(w_pfd) == -1)
    throw (std::runtime_error("pipe() Error"));
  fcntl(r_pfd[0], F_SETFL, O_NONBLOCK);
  fcntl(r_pfd[1], F_SETFL, O_NONBLOCK);
  fcntl(w_pfd[0], F_SETFL, O_NONBLOCK);
  fcntl(w_pfd[1], F_SETFL, O_NONBLOCK);
  ptr->r_pfd = r_pfd[0];
  ptr->w_pfd = w_pfd[1];
  char* script_name;
  std::string cgi_path = ptr->http_response_.file_path_;
  std::cout << "CGI_PATH: " << cgi_path << std::endl;
  ptr->prog_name_ = cgi_path;
  script_name = (char*)cgi_path.c_str();
  ptr->fd_type_ = CGI;
	ptr->is_forked_ = true;
  child_pid = fork();
  if (child_pid == -1){
    close(r_pfd[1]);
    close(r_pfd[0]);
    close(w_pfd[1]);
    close(w_pfd[0]);
    throw (std::runtime_error("fork() Error"));
  }
  else if (!child_pid){ //child
    close(r_pfd[0]);
    dup2(r_pfd[1], STDOUT_FILENO);
    close(r_pfd[1]);
    close(w_pfd[1]);
    dup2(w_pfd[0], STDIN_FILENO);
    close(w_pfd[0]);
    char* exec_file[3];
    if (cgi_path.find(".py") != std::string::npos)
      exec_file[0] = (char*)"/usr/local/bin/python3";
    else if (cgi_path.find(".php") != std::string::npos)
      exec_file[0] = (char*)"/usr/bin/php";
    else
      exec_file[0] = (char*)ptr->prog_name_.c_str();
    exec_file[1] = script_name;
    exec_file[2] = NULL;
    char** envp = getEnvs(ptr);
    // for (int i = 0; envp[i]; i++)
    //   std::cerr << "ENVP" << i << ": " << envp[i] << std::endl;
    // std::cerr << "\nCGI1: " << exec_file[0];
    // std::cerr << "\nCGI2: " << exec_file[1] << std::endl;
    if (execve(exec_file[0], exec_file, envp) == -1){//envp needed
      for (int i = 0; i < 5; i++)
        delete envp[del_arr[i]];
      delete [] envp;
      throw (std::runtime_error("execve() Error"));
    }
  }
  //parent
  close(r_pfd[1]);
  close(w_pfd[0]);
  Kqueue::unregisterReadEvent(ptr->client_fd_, ptr);
  Kqueue::registerWriteEvent(w_pfd[1], ptr); 
  Kqueue::registerReadEvent(r_pfd[0], ptr);
  ptr->cgi_pid_ = child_pid;
}
