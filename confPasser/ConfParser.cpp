# include "ConfParser.hpp"

/* public */

ConfParser::~ConfParser(){}
/**
 * @brief 싱글톤 패턴을 위해서 static 변수를 반환합니다.
 * @return ConfParser& 싱글톤 패턴을 위해서 static 변수를 반환합니다.
 */
ConfParser& ConfParser::getInstance(){
	static ConfParser conf_parser_;
	return conf_parser_;
}

void	ConfParser::configParseAll(int argc, char**argv){
	try{
		if (argc > 2)
				throw(std::runtime_error("You can only one conffile or default file"));
		if (argc == 2)
			setConfPath(argv[1]);//this is my conf_file
		confInit();
		refineDirective();
	}
	catch (std::exception &e){
		std::cerr << e.what() << "\n";
		throw(e);
	}
}

/**
 * @brief 파일이름이 있는 경우만 호출하는 conf파일 경로 초기화 함수.
 * @param argv conf 파일의 경로
 */
void	ConfParser::setConfPath(char* path){file_name_ = path;}

/**
 * @brief conf파일을 열어서 conf파일을 http_store 벡터에 저장해줍니다.
 * (http안에서 serv_block, serv_block안에 loc_block까지 다 채우기)
 * @details 각 conf파일의 구조에 맞게 클래스 내부에 블록들에 대한 저장을 가지고 있습니다.
 * 1. root에서 http 블록에 해당하는 모든 블록을 가지고 있고,
 * 2. http에서 모든 server 블록을
 * 3. server에서 모든 location을 가지고 있습니다.
 * 만약 http 블록이 없고 server block만 있는 경우 제가 self로 http 블록을 생성해주는 방식으로 갑니다.
 * (다만 http 블록은 한개만 가능하도록 하겠습니다.)
 * @warning 오류가 발생하면 invalid_argument가 throw 됩니다. (오류가 나도 close 처리는 해줬습니다.)
 */
void  ConfParser::confInit(){
	std::ifstream conf_file(file_name_.c_str());
	if (!conf_file.is_open())
		throw std::invalid_argument("Error : there is no conf_file\n");
	try{
		parseUntilEnd(conf_file, line_len_, *this);
	} catch(std::exception &e){
		std::cerr << file_name_ << " : " << line_len_ << "\n";
		std::cerr << e.what() << "\n";
		conf_file.close();
		throw(std::invalid_argument(""));
	}
	conf_file.close();
}

/**
 * @brief 이 함수를 호출함으로써 map에만 담긴 데이터들은 http, server location블록 모두 클래스에 필요한 데이터에 담김니다.
 */
void	ConfParser::refineDirective(){http_store_[0].refineAll();}

/**
 * @brief 포트번호와 host_name을 인자로 넣어주면 그에 해당하는 정보를 가진 서버 블록을 반환합니다.
 *
 * @param port_num 요청이 온 포트번호
 * @param serv_name http에서 host_name에 해당
 * @return ServBlock 해당하는 서버블록
 */
ServBlock	ConfParser::getServBlock(int port_num, std::string serv_name)const {return (http_store_[0].findServBlock(port_num, serv_name));}


/* 사용자가 호출하지 않는 public 함수 (재귀 템플릿 때문에 public.) */

/**
 * @brief 새로운 블록을 만드는 함수 블록은 오직 "http {"
 * 또는 "블록_이름 {" 로 들어오는 것만 올바른 블록이라 인식했습니다.
 *
 * @param line "블록 명{" 이렇게 들어온 한 줄 (이 함수에서는 인터페이스와 재귀를 위해서 사용하지 않는 인자를 넣어줬습니다.)
 * @param input 열어 ifstream으로 열어둔 conf파일
 */
void	ConfParser::makeBlock(std::string line, std::ifstream& input, int &line_len){
	(void)line_len;
	size_t pos = line.find('{');
	std::string block_name = line.substr(0, pos);
	trimSidesSpace(block_name);
	if (pos != line.size() - 1 || block_name == "")
		throw(std::runtime_error("block name in ERROR"));
	switch(checkBlockName(block_name)){
		case HTTP : makeHttpBlock(input);
			break;
		case SERVER : throw(std::runtime_error("You need to create an HTTP block first.(server)"));
		case LOCATION : throw(std::runtime_error("You need to create an HTTP block first.(location)"));
		case OTHERBLOCK : makeOtherBlock(input);
			break;
	}
}

/**
 * @brief root_directives_ 맵을 반환하는 함수
 *
 * @note 클래스 멤버를 레퍼런스로 반환(재귀 함수 사용 위해)
 * @return std::map<std::string, std::string>& 클래스 맴버인 map 레퍼런스
 */
std::map<std::string, std::string>& ConfParser::getDirStore(){return (root_directives_);}

/* private */


/**
 * @brief 기본 생성자가 호출되면 conf파일의 경로를 default.conf파일로 설정합니다.
 */
ConfParser::ConfParser():file_name_("./default.conf"), line_len_(0){}

/**
 * @brief HTTP 블록을 만드는 함수
 * @warning HTTP는 오직 1개의 블록만 올 수 있습니다.
 *
 * @param input 열려있는 conf_file
 */
void	ConfParser::makeHttpBlock(std::ifstream& input){
	if (http_store_.size() != 0)
		throw(std::runtime_error("there are two http block!!"));
	HttpBlock new_block;
	http_store_.push_back(new_block);
	parseUntilEnd(input, line_len_, http_store_[0]);
}

/**
 * @brief OtherBlock을 만드는 함수
 *
 * @param input 열려있는 conf_file
 */
void	ConfParser::makeOtherBlock(std::ifstream& input){
	OtherBlock new_block;
	other_store_.push_back(new_block);
	parseUntilEnd(input, line_len_, other_store_[other_store_.size() - 1]);
}
