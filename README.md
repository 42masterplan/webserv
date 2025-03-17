# Web Server Project | 웹서버 프로젝트
## Introduction | 소개

This project involves the creation of a web server that mimics the behavior of Nginx, adhering strictly to the HTTP 1.1 standards.
이 프로젝트는 HTTP 1.1 표준을 준수하면서 Nginx와 유사하게 동작하는 웹서버를 구현하는 것입니다.

## Installation | 설치 방법

Instructions to install and build the project are provided in the repository. Follow the steps to compile the server.
프로젝트를 설치하고 빌드하는 지침은 저장소에서 제공됩니다. 서버를 컴파일하기 위한 단계를 따르십시오.

## Usage | 사용 방법

To start the server, use the following command:
서버를 시작하려면 다음 명령어를 사용하세요:

```bash
./webserv [configuration file]
```
If no configuration file is specified, the server will use a default configuration file.
구성 파일이 지정되지 않은 경우 서버는 기본 구성 파일을 사용합니다.

If no configuration file is specified, the server will use a default configuration file.
구성 파일이 지정되지 않은 경우 서버는 기본 구성 파일을 사용합니다.

## Features
## 기능

- Non-blocking I/O operations using `kqueue`.
- `kqueue`을 사용한 논블로킹 I/O 작업.
- Supports HTTP methods GET, POST, and DELETE.
- HTTP 메서드 GET, POST, DELETE 지원.
- File upload capability.
- 파일 업로드 기능.
- Static website serving.
- 정적 웹사이트 서빙.
- Custom error pages for missing resources.
- 리소스가 없는 경우 사용자 정의 오류 페이지 제공.

## Configuration | 구성

The server configuration can be modified by editing the `config file`. Key settings include:
서버 구성은 `config 파일`을 편집하여 수정할 수 있습니다. 주요 설정은 다음과 같습니다:

- Multiple listening ports.
- 여러 리스닝 포트.
- Server name setup.
- 서버 이름 설정.
- Custom error pages.
- 사용자 정의 오류 페이지.
- Limit on client body size.
- 클라이언트 본문 크기 제한.
- Routing rules.
- 라우팅 규칙.
- HTTP redirections.
- HTTP 리디렉션.
- Directories and files for serving content.
- 콘텐츠 제공을 위한 디렉토리 및 파일.

## Dependencies | 의존성

Ensure you have the following dependencies installed:
다음 의존성이 설치되어 있는지 확인하십시오:

- C++98 compiler (e.g., GCC, Clang)
- C++98 컴파일러 (예: GCC, Clang)
- Standard C++98 and C libraries as mentioned in the project specifications.
- 프로젝트 사양에 언급된 표준 C++98 및 C 라이브러리.
- macOS operating system, as this program utilizes `kqueue`, which is specific to macOS.
- macOS 운영 체제, 이 프로그램은 macOS에 특화된 `kqueue`를 사용하기 때문입니다.
- To run on Linux, you need to change the code in kqueue to epoll or poll or select.
- Linux에서 실행하려면 kqueue의 코드를 epoll 또는 poll 또는 select로 변경해야 합니다.

## POST
[블로그 링크](https://haward.tistory.com/category/42Seoul/webserv)
