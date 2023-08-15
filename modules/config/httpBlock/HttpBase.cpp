# include "HttpBase.hpp"

HttpBase::HttpBase() :  root_(""), autoindex_(false), client_max_body_size_(0), error_page_(""){}
HttpBase:: ~HttpBase(){}

/*getter*/
const std::string& HttpBase::getRoot() const{return root_;}
const std::vector<std::string>& HttpBase::getIndex() const{return index_;}
const bool& HttpBase::isAutoIndex() const{return autoindex_;}
const int& HttpBase::getClientMaxBodySize() const{return client_max_body_size_;}
const std::vector<int>& HttpBase::getErrorCode() const{return error_code_;}
const std::string& HttpBase::getErrorPage() const{return error_page_;}

/**
 * @brief Root 까지 붙혀준 ErrorPage위치를 반환하는 함수
 *
 * @return std::string error_page 위치
 * @note 이 함수로 error_page의 경로를 찾습니다.
 */
std::string HttpBase::getCombineErrorPath()const{
	if (error_page_ == "")
		return (error_page_);
	return root_+error_page_;
}

/**
 * @brief http_base에 있는 멤버변수 정보를 보여주는 함수
 * @note 상속 받아서 모든 클래스 Block에서 사용
 */
void	HttpBase::printHttpInfo()const{
	std::cout << "\n---------------[http]------------------\n";
	std::cout << "root:|" << root_ << "|\n";
	std::cout << "[index]\n";
	for (size_t i = 0;i < index_.size(); i++){
		std::cout << "index[" << i << "]:|" << index_[i] << "|\n";
	}
	std::cout << "autoindex:|" << autoindex_ << "|\n";
	std::cout << "client_max_size:|" << client_max_body_size_ << "|\n";
	std::cout << "[error_code]\n";
	for (size_t i = 0;i < error_code_.size(); i++){
		std::cout << "error_code_[" << i << "]:|" << error_code_[i] << "|\n";
	}
	std::cout << "error_page:|" << error_page_ << "|\n";
}

/**
 * @brief 다른 HttpBase클래스의 멤버변수를 가져오는 함수입니다.
 * @note 주로 부모 클래스의 내용을 상속을 받을 때 사용합니다.
 * @param base 업캐스팅된 부모 블록 클래스
 */
void	HttpBase::setInherit(HttpBase &base){
	autoindex_ = base.autoindex_;
	root_ = base.root_;
	index_ = base.index_;
	autoindex_ = base.autoindex_;
	client_max_body_size_ = base.client_max_body_size_;
	error_code_ = base.error_code_;
	error_page_ = base.error_page_;
}

/* Private */
/**
 * @brief map에 잘려있는 값들을 사용할 수 있게 정제해준다.
 * @param dir_store 해당하는 map값
 */
void	HttpBase::parseHttpDirective(std::map<std::string, std::string>& dir_store){
	if (index_.size() == 0) 
		index_.push_back("");
	if (dir_store.find("root") != dir_store.end())
		root_ = dir_store["root"];
	if (dir_store.find("index") != dir_store.end())
		splitAndStore(index_,dir_store["index"],' ');
	if (dir_store.find("autoindex") != dir_store.end())
		setAutoIndex(dir_store["autoindex"]);
	if (dir_store.find("client_max_body_size") != dir_store.end())
		client_max_body_size_ = stringToInt(dir_store["client_max_body_size"]);
	if (dir_store.find("error_page") != dir_store.end())
		 setErrorPage(dir_store["error_page"]);
}

/**
 * @brief Autoindex를 설정합니다.
 * @param value key value로 잘린 map에서 value값
 */
void	HttpBase::setAutoIndex(const std::string& value){
	if (value == "on")
			autoindex_ = true;
	else if (value == "off")
		autoindex_ = false;
	else
		throw(std::invalid_argument("ERROR in AUTO index\n"));
}

/**
 * @brief 에러페이지를 설정합니다.
 * @param line 에러페이지의 status 코드와 경로가 있는 헌쥴울 전달해준다.
 * @note 거꾸로 탐색합니다. 뒷쪽에 에러페이지는 하나만 에러코드는 여러개 올 수 있습니다.
 */
void	HttpBase::setErrorPage(const std::string& line){
	int i;
	for (i = line.size() - 1;i  > 0; i--){
		if (line[i] == ' ')
			break;
	}
	if (i == 0)
		throw(std::runtime_error("ERROR_PAGE SETING ERROR!!"));
	splitAndStore(error_code_,  line.substr(0, i), ' ');
	error_page_ = line.substr(i+1);
}
