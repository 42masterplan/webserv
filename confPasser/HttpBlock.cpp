# include "HttpBlock.hpp"
HttpBlock::HttpBlock(){}
HttpBlock::~HttpBlock(){}

/**
 * @brief http의 모든 directive를 정제하는 함수 동시에 가지고 있는 모든 server_block에게 정제 명령을 내립니다.
 * @todo 같은 포트에 대해서 에러를 처리하는 코드를 주석해뒀습니다.(나중에 추가될 수 있는 사항)
 */
void	HttpBlock::refineAll(){
	// std::vector<int> check_same;
	parseHttpDirective(http_directives_);
	if (serv_store_.size() == 0)
		throw(std::runtime_error("You must input Server block least One Block!"));
	for (size_t i = 0; i < serv_store_.size(); i++){
		serv_store_[i].setInherit(*this);
		serv_store_[i].refineAll();
		// check_same.push_back(serv_store_[i].getListen());
	}
	// std::sort(check_same.begin(), check_same.end());
	// int bef = -1;
	// for (size_t i = 0; i < check_same.size(); i++){
	// 	if (check_same[i] == bef)
	// 		throw(std::runtime_error("Cannot Same_Port NUM SERVER!!"));
	// 		bef = check_same[i];
	// }
}

/**
 * @brief 서버이름 즉 http의 host_name을 기준으로 해당하는 서버를 찾는 함수
 * @param port_num 포트번호에 해당하는 값을 전달
 * @param serv_name http메세지에서 host_name
 * @return ServBlock 해당하는 서버 블록 또는 default 서버는 port 번호가 같으면 그걸로 가는것 입니다.(포트가 다르면 짤.)
 * @warning 포트가 일치하는 server_block이 없는 경우 listen이 -1인 server block을 리턴합니다.
 */
ServBlock HttpBlock::findServBlock(int port_num, std::string serv_name)const {
	int default_serv = -1;
	ServBlock not_allow_server;
	for (size_t i = 0; i < serv_store_.size(); i++){
		if (serv_store_[i].getListen() == port_num){
			if (default_serv == -1)
				default_serv = i;
			const std::vector<std::string> &serv_names = serv_store_[i].getServerName();
			for (size_t j = 0; j < serv_names.size(); j++){
				if (serv_names[j] == serv_name)
					return (serv_store_[i]);
			}
		}
	}
	if (default_serv != -1)
		return (serv_store_[default_serv]);
	return (not_allow_server); // 포트가 다른 경우 listen 이 -1인 Serv block을 리턴합니다.
}

/**
 * @brief HTTP Block에서 정보를 보고 싶을 때 사용가능
 * @note 사실 printHttpInfo() 함수를 호출하는 것임.
 */
void	HttpBlock::printInfo()const{printHttpInfo();}

/* 사용자가 호출하지 않는 public 함수 (재귀 템플릿 때문에 public.) */
/**
 * @brief http_directives_ 반환
 * @return std::map<std::string, std::string>& 인자로 받아서 변경시키기 위해서 레퍼런스 타입으로 반환함
 */
std::map<std::string, std::string>& HttpBlock:: getDirStore(){return (http_directives_);}

/**
 * @brief 블록을 만드는 함수 서버 블록과 otherBlock허용
 *
 * @param line 파싱중인 한줄 라인(주석과 스페이스를 삭제한)
 * @param input 파싱중인 설정파일
 * @param line_len_ 설정파일을 어디까지 읽었는지 확인
 * @note 템플릿 사용을 위해 반드시 필요
 * @warning  http, location 블록은 에러로 감지함
 */
void	HttpBlock::makeBlock(std::string line, std::ifstream& input, int& line_len_){
	size_t pos = line.find('{');
	std::string block_name = line.substr(0, pos);
	trimSidesSpace(block_name);
	if (pos != line.size() - 1 || block_name == "")
		throw(std::runtime_error("this is not block"));
	switch(checkBlockName(block_name)){
		case HTTP :throw(std::runtime_error("this is not GOOD HTTP block"));
		case SERVER : makeServerBlock(input, line_len_);
			break;
		case LOCATION : throw(std::runtime_error("this is not LOC block"));
		case OTHERBLOCK : makeOtherBlock(input, line_len_);
			break;
	}
}

/* private */
/**
 * @brief 서버블록을 만드는 함수
 *
 * @param input 열어둔 설정파일
 * @param line_len_ 설정파일 읽은 길이
 * @note 재귀적으로 돌기 위한 템플릿 함수 parseUntilEnd를 호출한다.
 */
void	HttpBlock::makeServerBlock(std::ifstream& input, int& line_len_){
	ServBlock new_block;
	serv_store_.push_back(new_block);
	parseUntilEnd(input, line_len_, serv_store_[serv_store_.size() - 1]);
}

/**
 * @brief Other 블록을 만드는 함수
 *
 * @param input 열어둔 설정파일
 * @param line_len_ 설정파일 읽은 길이
 * @note 재귀적으로 돌기 위한 템플릿 함수 parseUntilEnd를 호출한다.
 */
void	HttpBlock::makeOtherBlock(std::ifstream& input, int& line_len_){
	OtherBlock new_block;
	other_store_.push_back(new_block);
	parseUntilEnd(input, line_len_, other_store_[other_store_.size() - 1]);
}
