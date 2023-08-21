#ifndef CONF_PARSER_HPP
# define CONF_PARSER_HPP
# include "./httpBlock/HttpBlock.hpp"
# include "./interface/IBlock.hpp"
/**
 * @brief 루트에 해당하는 클래스라고 보면 됩니다.
 * @details 사용할 지시어가 올바른 위치에 있지 않는 것에 대한 에러는 처리하지 않습니다.
 * @note  모든 블록 클래스는 makeBlock과 getDirStore 함수를 가지고 있습니다.
 * 템플릿으로 재귀적으로 처리하기 위해서.
 * @details [conf파일에서 root 붙혀줘야되는 부분]
 * 1. error_page [o]->http base (모든 블록, location에서 virtual)
 * 2. upload_store[o](server, location block)
 * 3. cgi_pass [o](location)
 * 4. return path[o](location)
 * 5. location 접근 path[](location)
 */
class ConfParser : public IBlock{

public:
	static ConfParser& getInstance();
	virtual ~ConfParser();

	void	configParseAll(int argc, char**argv);
	const std::vector<int>& getListenPorts()const;
	void	setConfPath(char* path);
	void	confInit();
	void	refineDirective();
	ServBlock	getServBlock (int port_num, std::string serv_name)const;
	//사용자가 직접 호출할 일 없는 함수들 (for template)
	void	makeBlock(std::string line, std::ifstream& input, int& line_len);
	std::map<std::string, std::string>& getDirStore();
private:
	ConfParser();
	void	makeHttpBlock(std::ifstream& input);
	void	makeOtherBlock(std::ifstream& input);

	std::string	file_name_;//conf 파일 파일이름
	int	line_len_;//conf파일 몇번 째 줄까지 읽었는지 확인하는 변수
	std::vector<HttpBlock>	http_store_;
	std::vector<OtherBlock>	other_store_;
	std::map<std::string, std::string>	root_directives_;//key : directive, value : value
};
#endif

