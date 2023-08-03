# include "ParseTool.hpp"
# include "../otherBlock/OtherBlock.hpp"
/**
 * @brief 어떤 블록인지 리턴해주는 함수
 *
 * @details switch 문 쓰려고 만들었어요
 * @param block_name 블록 이름
 * @return e_block ENUM 값으로 리턴해준다.
 */
e_block	checkBlockName(const std::string& block_name){
	if (block_name == "http")
		return (HTTPBLOCK);
	if (block_name == "server")
		return (SERVERBLOCK);
	if (block_name.find("location") != std::string::npos)
		return (LOCATIONBLOCK);
	return (OTHERBLOCK);
}

/**
 * @brief 메소드 종류를 리턴해주는 함수
 *
 * @param block_name 메소드의 이름을 string으로 받습니다.
 * @return e_method enum값을 가집니다.
 * @note GET, POST, HEAD, PUT, DELETE, OTHER_METHOD
 */
e_method checkMethodName(const std::string &block_name){
	if (block_name == "GET")
		return GET;
	if (block_name == "POST")
		return POST;
	if (block_name == "HEAD")
		return HEAD;
	if (block_name == "PUT")
		return PUT;
	if (block_name == "DELETE")
		return DELETE;
	return OTHER_METHOD;
}


/**
 * @brief double값으로 인자를 받는 int overflow 체크 함수
 *
 * @param d 체크하고자 하는 double값
 */
void  checkOverFlow(double d){
  int a = d;
  if (static_cast<double> (a) != std::floor(d))
    throw std::overflow_error("TOO_LARGENUM_ERROR");
}

/**
 * @brief 들어온 문자열의 양옆에 있는 white space을 지워주는 함수
 *
 * @param line 변경할 문자열
 */
void	trimSidesSpace(std::string &line){
	size_t i;
	//앞쪽의 white space를 지운다.
	if (line == "")
		return ;
	for (i = 0; i < line.size();	i++){
		if (line[i] != '\t' && line[i] != ' ' && line[i] != '\n' \
				&& line[i] != '\r' && line[i] != '\f' && line[i] != '\v')
			break;
	}
	line.erase(0, i);
	//뒤쪽의 white space를 지운다.
	if (line == "")
		return ;
	for (i = line.size() - 1; i > 0; i--){
		if (line[i] != '\t' && line[i] != ' ' && line[i] != '\n' \
				&& line[i] != '\r' && line[i] != '\f' && line[i] != '\v')
			break ;
	}
	line.erase(line.begin() + i + 1, line.end());
}

/**
 * @brief #으로 시작하는 comment를 전부 지워준다.
 *
 * @param line 변경할 문자열
 */
void	trimComment(std::string &line){
	size_t tmpos = line.find('#');
	if (tmpos != std::string::npos){
		line.erase(tmpos,line.size());
		line = line.substr(0, tmpos);
	}
}

/**
 * @brief line으로 들어온 것을 key와 value로 만듭니다.
 *
 * @param key key를 저장할 문자열 -함수 안에서 string을 변경합니다.
 * @param value value를 저장할 문자열 -함수 안에서 string을 변경합니다.
 * @param line key value로 쪼개고 싶은 한줄을 인자로 줍니다.
 * @warning derective를 key space value 형식이 아니면 runtime_error를 던집니다.
 */
void	splitKeyVal(std::string& key, std::string &value, std::string &line){
	size_t tmpos = line.find(' ');
	if (tmpos == std::string::npos)	//directive<space>value로 되어있어야 한다.
		throw(std::runtime_error("ERROR DIRECTIVE key"));
	key = line.substr(0,tmpos);
	// std::cout << "key:|"<< key << "|" << std::endl;
	for (; tmpos < line.size(); tmpos++){
		if (line[tmpos] != ' ' && line[tmpos] != '\t')
			break ;
	}
	value = line.substr(tmpos);
	// std::cout << "value:|"<< value << "|" << std::endl;
}

/**
 * @brief string을 split해서 string vector에 담아주는 함수
 *
 * @param store 저장할 벡터
 * @param line 자를 한줄
 * @param delimiter 구분자
 * @warning 들어온 vector에 대해서 clear를 진행한다.
 */
void	splitAndStore(std::vector<std::string>& store, std::string line, char delimiter){
	trimSidesSpace(line);
	if (line == "")
		return ;
	store.clear();
  std::stringstream ss(line);
  std::string temp;
	while (getline(ss, temp, delimiter)) {
		store.push_back(temp);
	}
}

/**
 * @brief string을 split해서 int vector에 담아주는 함수
 *
 * @param store 저장할 벡터
 * @param line 자를 한줄
 * @param delimiter 구분자
 * @warning 숫자가 아닌경우와 overflow에 대해서 에러를 처리합니다.
*  @warning 들어온 vector에 대해서 clear를 진행한다.
 */
void	splitAndStore(std::vector<int>& store, std::string line, char delimiter){
	store.clear();
	trimSidesSpace(line);
	std::stringstream ss(line);
  std::string temp;
	while (getline(ss, temp, delimiter)){
		store.push_back(stringToInt(temp));
	}
}

/**
 * @brief 앞뒤로 스페이스바 삭제해주고 Directive를 추출합니다.
 * 이후 정상적인 코드라고 생각하고 그 값을 파싱해줍니다.
 * 여러개인 경우 space로 띄워져 있습니다.
 *
 * @param line ';'로 잘린 문자열이 들어옵니다.
 * @param directives_map-클래스에서 directive들을 저장할 저장소
 */
void  extractDirective(std::string line, std::map<std::string, std::string>& directives_map){

	std::string key, value;
	splitKeyVal(key, value, line);
	directives_map[key] = value;
}

/**
 * @brief string을 int로 바꾸는 함수
 *
 * @param num string으로 생긴 숫자
 * @return int 변경된 int값
 * @warning 숫자가 아닌 값이 있거나, overflow가 나는 경우 throw해줍니다.
 */
int stringToInt(const std::string &num){
	double ret = 0;
	for (std::string::const_iterator it = num.begin(); it != num.end(); it++){
			if (*it < '0' || *it > '9')
				throw(std::invalid_argument("NUMBER_INVALID_FORMAT"));
			ret *= 10;
			ret += *it - '0';
	}
	checkOverFlow(ret);
	return ret;
}

/**
 * @brief vector<char>에서 CRLF의 위치를 찾습니다.
 * 
 * @param raw_data 
 * @return size_t CRLF이 시작하는 index를 반환합니다.
 * @warning CRLF가 존재하지 않을 때 vector<char>의 size를 반환합니다.
 */
size_t findCRLF(const std::vector<char>& raw_data) {
	for (size_t i = 0; i < raw_data.size() - 1; i++) {
		if (raw_data[i] == '\r' && raw_data[i + 1] == '\n')
			return i;
	}
	return raw_data.size();
}

/**
 * @brief findCRLF()를 이용하여 CRLF의 존재 여부를 확인합니다.
 * 
 * @param raw_data 
 * @return true : CRLF 존재함
 * @return false : CRLF 존재하지 않음
 */
bool hasCRLF(const std::vector<char>& raw_data) {
  size_t  split_idx;

  split_idx = findCRLF(raw_data);
	if (split_idx == raw_data.size())
    return false;
  return true;
}

/**
 * @brief 인자로 들어온 string을 모두 lower-case로 만들어주는 함수입니다.
 * 
 * @return std::string 
 */
std::string	lowerString(std::string& str) {
	std::string	lowerStr = "";

	for (size_t i = 0; i < str.length(); i++)
		lowerStr.push_back(tolower(str[i]));
	return lowerStr;
}

/**
 * @brief 인자로 들어온 두 개의 string을 case-insensitive하게 비교해주는 함수입니다.
 * 
 * @return true 
 * @return false 
 */
bool	insensitiveCompare(std::string& str1, std::string& str2) {
	return lowerString(str1) == lowerString(str2);
}
