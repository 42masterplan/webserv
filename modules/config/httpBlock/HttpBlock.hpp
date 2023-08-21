#ifndef HTTP_BLOCK_HPP
# define HTTP_BLOCK_HPP
# include "../serverBlock/ServBlock.hpp"
# include "../interface/IBlock.hpp"
# include "HttpBase.hpp"
/**
 * @brief HTTP block의 정보를 담고있는 블록. 내부에 서버 블록들을 가지고 있다.
 * @details [서버 네임이 둘 이상 일치하는 경우 우선순위]
		1. 정확한 이름
		2. 별표로 시작하는 가장 긴 와일드카드 이름(예: "*.example.org")
		3. 별표로 끝나는 가장 긴 와일드카드 이름(예: "mail.*")
		저희 과제에서는 와일드 카드 안 해서 둘 이상 일치하는 경우 가장 먼저 나오는 경우만 하겠습니다.
 * @details [와일드 카드]
		- 와일드 카드 규칙 : 와일드 카드는 반드시 시작과 끝에만 붙일 수 있습니다.
		- 스페셜 와일드 카드 (`.example.org`) : 이 와일드 카드는 `example.org` 로 정확한 서버이름을 정하기도 하고 `*.example.org` 를 의미하기도 합니다.
 * @details [서버 블록 찾기 규칙]
		1. 포트 번호를 기준으로 쭉 탐색한다.
		2. 같은 포트번호가 있다면 그 포트번호에 해당하는 서버들을 본다.
		3. 그리고 서버네임을 확인해서 해당하는 서버네임 으로 호스팅 해준다.
			a. `nginx`는 먼저 서버 블록의 `listen 지시어`에 대해 `요청의 IP 주소와 포트를 테스트`합니다.
			b. 그런 다음 요청의 `"호스트" 헤더 필드`를 `IP 주소` 및 `포트와 일치하는 서버 블록`의 `server_name` 항목과 비교하여 테스트합니다.
			c. 서버 이름을 찾을 수 없는 경우 요청은 `기본 서버`에서 처리됩니다.
 * @note [http 블록 주요 인자] root, index, autoindex, error_page, client_max_body_size
 * @details [인자별 상세 설명]
 * root : 인자 1개, http 블록 부터 나올 수 있다.
 * index : 인자 여러개 'space'기준,
 * autoindex : http부터 on 또는 off
 * error_page(예시) : error_page 404 502 503 504 /errorPages/404.html;
 * client_max_body_size(2개면 에러긴 하다.) : 이건 int로 크기를 지정해서 1당 1바이트로 생각하겠습니다.
 * - 클라이언트 요청 본문의 최대 허용 크기를 설정합니다. 
 * - 요청의 크기가 구성된 값을 초과하면 413(요청 엔터티가 너무 큼) 오류가 클라이언트에 반환됩니다. 
 * - 브라우저에서는 이 오류를 올바르게 표시할 수 없습니다. 0으로 설정하면 *`size`*클라이언트 요청 본문 크기 확인이 비활성화됩니다.
 *
 */
class HttpBlock : public IBlock, public HttpBase{
public:
	HttpBlock();
	virtual ~HttpBlock();

	const std::vector<int>& getListenPorts()const;
	void	refineAll();
	ServBlock findServBlock(int port_num, std::string serv_name)const;
	void	printInfo()const;

	/*사용자가 직접 호출할 일 없는 함수들 (for template)*/
	std::map<std::string, std::string>& getDirStore();
	void	makeBlock(std::string line, std::ifstream& input, int& line_len_);
private:
	void	makeServerBlock(std::ifstream& input, int& line_len_);
	void	makeOtherBlock(std::ifstream& input, int& line_lne_);

	std::vector<ServBlock> serv_store_;
	std::vector<OtherBlock> other_store_;
	std::map<std::string, std::string> http_directives_;
	std::vector<int> port_store_;
};

#endif
