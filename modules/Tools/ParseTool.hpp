#ifndef PARSE_TOOL_HPP
# define PARSE_TOOL_HPP

# include "./FileTool.hpp"

typedef enum methodType{
	GET,
	HEAD,
	DELETE,
	POST,
	PUT,
	PATCH,
	OTHER_METHOD
}e_method;

//지원하지 않는 메서드
// OPTIONS, //서버가 지원하는 메서드를 판별할 수 있는 명령
// COPY, //
// MOVE,
// PROPFIND,
// PROPPATCH,
// LOCK,
// UNLOCK,
// MKCOL //디렉토리 만드는 method

typedef enum blockType{
	HTTPBLOCK,
	SERVERBLOCK,
	LOCATIONBLOCK,
	OTHERBLOCK
}e_block;

/* ConfParser utils */
e_block	checkBlockName(const std::string& block_name);
e_method checkMethodName(const std::string &block_name);

void  checkOverFlow(double d);

void	trimSidesSpace(std::string &line);
void	trimComment(std::string &line);

void	splitKeyVal(std::string& key, std::string &value, std::string &line);
void	splitAndStore(std::vector<std::string>& store, std::string line, char delimiter);
void	splitAndStore(std::vector<int>& store, std::string line, char delimiter);

void  extractDirective(std::string line, std::map<std::string, std::string>& directives_map);

int		stringToInt(const std::string &num);
bool	isExist(size_t pos);




/**
 * @brief [파싱에서 가장 핵심 함수] 재귀적으로 괄호가 닫힐때까지 탐색하는 함수
 * 탬플릿으로 블록을 받고 각 블록 내부의 함수를 활용해서 재귀적으로 타고 갑니다.
 * @details [블록 파싱 규칙]
 * 1. 블록은 "server {"  이런식으로 생긴 것만 생각합니다.
 * [ex]
 * server
 * # hi
 * # hello
 * {
 * }
 * 이런식으로 되있는 경우는 아에 안되는 것으로 처리하겠습니다.
 * ex : server{location{}} 이렇게 오는것도 안 합니다.
 *
 * @tparam T (ConfParser, HttpBlock, ServBlock, OtherBlock, locBlock) 클래스가 들어옵니다.
 * @warning 호출 하기 전에 T block 안에 makeBlock() 함수가 있는지 확인합니다.
 * @param input conf파일을 열어 놓은 ifstream 입니다.
 * @param line_len_ ConfParser에서 있는 멤버변수입니다. 에러가 어디서 났는지 알기위해 계속 가져갑니다.
 * @param block 새로 만든 즉 벡터에 들어갈 블록이 들어옵니다.
 */
template <typename T>
void	parseUntilEnd(std::ifstream& input, int& line_len_, T& block){
	std::string line;

	while (std::getline(input, line)){
		trimComment(line);
		trimSidesSpace(line);
		line_len_++;
		if (line == "")
			continue;
		size_t pos_brace_open = line.find('{');
		size_t pos_semicolon = line.find(';');
		size_t pos_brace_close = line.find('}');
		if (isExist(pos_brace_close)){ //괄호가 닫히는 경우 재귀 종료
			if (line.size() != 1)
				throw(std::runtime_error(" '{' is only line by line"));
			return ;
		}
		else if ((!isExist(pos_brace_open) && !isExist(pos_semicolon)) || \
					(isExist(pos_brace_open) && isExist(pos_semicolon)))
			throw(std::runtime_error(" [ERROR in Nginx conf_file]"));
		else if (isExist(pos_semicolon) && !isExist(pos_brace_open))
				extractDirective(line.substr(0, pos_semicolon), block.getDirStore());
		else{ // {가 나오는 경우
			block.makeBlock(line, input, line_len_);
			if (input.eof() == true)
				throw(std::runtime_error("this is not close {"));
		}
	}
	if (input.eof() == false) //시작할 때 괄호가 모두 닫혀있지 않다면
		throw(std::runtime_error("NOT CLOSE the {}"));
}

/* HttpParser utils */
size_t				findCRLF(const std::vector<char>& raw_data);
bool					hasCRLF(const std::vector<char>& raw_data);
void					lowerString(std::string& str);
bool					insensitiveCompare(std::string str1, std::string str2);
std::vector<std::string>	split(std::string input, std::string delimiter);


std::string		intToString(int num);
void					print_vec(const std::vector<char>& vec);


#endif
