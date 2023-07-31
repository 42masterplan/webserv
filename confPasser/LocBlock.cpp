#include "LocBlock.hpp"


LocBlock::LocBlock(std::string loc_info) : rank_(0), upload_store_(""), loc_info_(loc_info), return_code_(-1), return_path_("") ,cgi_pass_(""),  is_limit_except_(false),combined_path_(""),high_priority_root_(""){}

LocBlock::~LocBlock(){}
// LocBlock& LocBlock::operator=(const LocBlock& obj){
// 	root_ = obj.root_;
// 	index_ = obj.index_;
// 	autoindex_ = obj.autoindex_;
// 	client_max_body_size_ = obj.client_max_body_size_;
// 	error_code_ = obj.error_code_;
// 	error_page_ = obj.error_page_;

// 	loc_directives_ = obj.loc_directives_;
// 	upload_store_ = obj.upload_store_;
// 	loc_info_ = obj.loc_info_;
// 	return_code_ = obj.return_code_;
// 	return_path_ = obj.return_path_;
// 	is_limit_except_ = obj.is_limit_except_;
// 	cgi_pass_ = obj.cgi_pass_;
// 	deny_methods_ = obj.deny_methods_;
// 	combined_path_ = obj.combined_path_;
// 	return (*this);
// }

/*----------------getter (멤버변수 그대로 반환)---------------*/
const std::string& LocBlock::getUploadStore()const{return upload_store_; }
const std::string& LocBlock::getLocInfo()const{return loc_info_;}
const int& LocBlock::getReturnCode()const{return return_code_;}
const std::string& LocBlock::getReturnPath()const{return return_path_;}
const std::string& LocBlock::getCgiPath()const{return cgi_pass_;}
const int& LocBlock::getRank() const {return rank_;}

/**
 * @brief 거부하는 메서드가 있는지 확인하는 함수
 * @return true 거부하는 메서드가 있다.
 * @return false 거부하는 메서드가 없다.
 */
const bool& LocBlock::isLimit()const{return is_limit_except_;}

/**
 * @brief 거부하는 메서드를 벡터로 반환
 * @note 이걸 사용하기 전에 isLimit() 함수 호출로 확인해봅시다.
 *
 * @return const std::vector<std::string>&
 */
const std::vector<std::string>& LocBlock::getDenyMethod()const{return deny_methods_;}


/*----------실제 사용할 경로를 찾아줄 getter (for user)-----------*/
/**
 * @brief error_page경로를 root를 붙혀서 반환하는 함수
 * @return std::string root를 붙힌 error_page 경로
 */
std::string LocBlock::getCombineErrorPath()const{
	if (error_page_ == "")
		return (error_page_);
	if (high_priority_root_ == "")
		return(root_ + error_page_);
	return (high_priority_root_ + error_page_);
}

/**
 * @brief upload_store경로를 root를 붙혀서 반환하는 함수
 * @return std::string root를 붙힌 upload_store 경로
 */
std::string LocBlock::getCombineUploadStorePath()const{
	if (upload_store_ == "")
		return (upload_store_);
	if (high_priority_root_ == "")
		return(root_ + upload_store_);
	return (high_priority_root_ + upload_store_);
}

/**
 * @brief 리다이렉션 경로를 root를 붙혀서 반환하는 함수
 * @return std::string root를 붙힌 리다이렉션 경로
 */
std::string LocBlock::getCombineReturnPath()const{
	if (return_path_ == "")
		return (return_path_);
	if (high_priority_root_ == "")
		return(root_ + return_path_);
	return (high_priority_root_ + return_path_);
}

/**
 * @brief cgi경로를 root를 붙혀서 반환하는 함수
 * @return std::string root를 붙힌 cgi의 경로
 */
std::string LocBlock::getCombineCgiPath()const{
	if (cgi_pass_ == "")
		return (cgi_pass_);
	if (high_priority_root_ == "")
		return(root_ + cgi_pass_);
	return (high_priority_root_ + cgi_pass_);
}

/**
 * @brief 로케이션 블럭이 index를 거쳐서 붙은 root경로까지 붙힌 완성된 파일 경로
 * @return const std::string& 실제로 접근해서 확인해볼 경로
 */
const std::string& LocBlock::getCombineLocPath()const{return (combined_path_);}

/*--------setter-------------*/
void	LocBlock::setCombinePath(std::string Combine_path){combined_path_ = Combine_path;}
void	LocBlock::setHighPriorityRoot(const std::string & root){high_priority_root_ = root;}

/**
 * @brief 반환된 블럭이 에러인지 아닌지 여부 확인
 *
 * @return true 해당하는 location block이 없어서 나온 에러 블럭입니다.
 * @return false 에러 블럭이 아닙니다.
 */
bool LocBlock::isErrorBlock()const{
	if (combined_path_ == "")
		return true;
	return false;
}

/**
 * @brief location block의 모든 directive를 정제하는 함수
 * @note location block의 rank를 설정해준다.
 */
void	LocBlock::refineAll(){
	parseHttpDirective(loc_directives_);
	parseLocDirective();
	if (loc_info_ == "/")
		return ;
	for (size_t i = 0;i < loc_info_.size(); i++){
		if (loc_info_[i] == '/')
			rank_++;
	}
}

/**
 * @brief location block 멤버변수 출력하는 함수(http info까지 같이)
 */
void	LocBlock::printInfo()const{
	printHttpInfo();
	std::cout << "\n---------------[Location]------------------\n";
	std::cout << "rank_:|" << rank_ << "|\n";
	std::cout << "upload_store_:|" << upload_store_ << "|\n";
	std::cout << "loc_info_:|" << loc_info_ << "|\n";
	std::cout << "return_code_:|" << return_code_ << "|\n";
	std::cout << "return_path_:|" << return_path_ << "|\n";
	std::cout << "is_limit_except_:|" << is_limit_except_ << "|\n";
	std::cout << "cgi_pass_:|" << cgi_pass_ << "|\n";
	std::cout << "[deny_methods_]\n";
	for (size_t i = 0;i < deny_methods_.size(); i++){
			std::cout << "deny_methods_[" << i << "]:|" << deny_methods_[i] << "|\n";
	}
	std::cout << "[Print All Combine Path]\n";
	std::cout << "getCombineErrorPath:|" << getCombineErrorPath()<< "|\n";
	std::cout << "getCombineUploadStorePath:|" << getCombineUploadStorePath()<< "|\n";
	std::cout << "getCombineReturnPath:|" << getCombineReturnPath()<< "|\n";
	std::cout << "getCombineCgiPath:|" << getCombineCgiPath()<< "|\n";
	std::cout << "getCombineLocPath:|" << getCombineLocPath()<< "|\n";
}


/* -------사용자가 호출하지 않는 public 함수 (재귀 템플릿 때문에 public.) ----------*/
/**
 * @brief location block에서는 limit_except 블록만 올 수 있습니다.
 *
 * @param line 반드시 "limit_except GET POST HEAD {" 이런 느낌으로 옵니다.
 * @param input 열어둔 파일시스템
 * @param line_len_ conf파일 현재까지 읽은 길이
 *
 * @warning limit_except 에 올 수 있는 메서드는 임의롤 줄였습니다.
 * 추가적으로 deny all이외에 다른 인자는 받지 않습니다.
 */
void	LocBlock::makeBlock(std::string line, std::ifstream& input, int& line_len_){
	std::string key,value;
	if (line.find("limit_except") == std::string::npos)
		throw(std::runtime_error("CAN't Make block in Loc block!(only allow limit_except)"));
	is_limit_except_ = true;
	splitAndStore(deny_methods_,line.substr(12, line.find('{') - 12) ,' ');
	for (std::vector<std::string>::iterator it = deny_methods_.begin(); it != deny_methods_.end(); it++){
		if (checkMethodName(*it) == OTHER_METHOD)
			throw(std::runtime_error("THIS IS NOT ALLOW METHOD!"));
	}
	while (getline(input, line)){
		trimComment(line);
		trimSidesSpace(line);
		if (line == "")
			continue;
		if (line == "}")
			return;
		line = line.substr(0, line.find(';'));
		line_len_++;
		splitKeyVal(key, value, line);
		if (key != "deny" ||  value != "all")
			throw(std::runtime_error("ERROR it must only deny all"));
	}
	throw(std::runtime_error("Not closed by }"));
}

/**
 * @brief loc_directives_ 반환
 *
 * @note 템플릿 사용 위해서 반드시 필요
 * @return std::map<std::string, std::string>& 인자로 받아서 변경시키기 위해서 레퍼런스 타입으로 반환함
 */
std::map<std::string, std::string>& LocBlock::getDirStore(){return (loc_directives_);}


/* private */
LocBlock::LocBlock(){}

/**
 * @brief location 블록에만 해당하는 값들을 정제
 */
void	LocBlock::parseLocDirective(){
	std::map<std::string, std::string>::iterator it = loc_directives_.find("upload_store");
	if (it != loc_directives_.end())
		upload_store_ = (*it).second;
	it = loc_directives_.find("return");
	if (it != loc_directives_.end())
		parseReturn((*it).second);
	it = loc_directives_.find("cgi_pass");
	if (it != loc_directives_.end())
		cgi_pass_ = (*it).second;
}

/**
 * @brief return에 해당하는 부분을 파싱 (redirection과 연관있습니다.)
 *
 * @param ret_line return key를 가진 directive의 value 값 입니다.(map["return"];)
 */
void	LocBlock::parseReturn(std::string ret_line){
	std::vector<std::string> tmp;
	splitAndStore(tmp, ret_line, ' ');
	if (tmp.size() != 2)
		throw(std::runtime_error("you must return argument only two!"));
	return_code_ = stringToInt(tmp[0]);
	return_path_ = tmp[1];
}

/**
 * @brief rank를 기반으로 sort 하기 위해서 있는 외부 함수
 */
bool cmp(const LocBlock& a, const LocBlock& b){
	return (a.getRank() > b.getRank());
}
