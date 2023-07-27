#ifndef CONFPARSER_HPP
# define CONFPARSER_HPP
# include "HttpBlock.hpp"
# include "IBlock.hpp"

class ConfParser : public IBlock{

public:
	ConfParser();
	virtual ~ConfParser();

	void	setConfPath(char* path);
	void	confInit();
	void	refineDirective();
	ServBlock	getServBlock (int port_num, std::string serv_name)const;

	//사용자가 직접 호출할 일 없는 함수들 (for template)
	void	makeBlock(std::string line, std::ifstream& input, int& line_len);
	std::map<std::string, std::string>& getDirStore();
private:
	void	makeHttpBlock(std::ifstream& input);
	void	makeOtherBlock(std::ifstream& input);

	std::string	file_name_;//conf 파일 파일이름
	int	line_len_;//conf파일 몇번 째 줄까지 읽었는지 확인하는 변수
	std::vector<HttpBlock>	http_store_;
	std::vector<OtherBlock>	other_store_;
	std::map<std::string, std::string>	root_directives_;//key : directive, value : value
};
#endif