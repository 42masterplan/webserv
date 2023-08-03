# include "OtherBlock.hpp"

OtherBlock:: OtherBlock(){}
OtherBlock:: ~OtherBlock(){}

/**
 * @brief other_directives_ 반환
 *
 * @note 템플릿 사용 위해서 반드시 필요
 * @return std::map<std::string, std::string>& 인자로 받아서 변경시키기 위해서 레퍼런스 타입으로 반환함
 */
std::map<std::string, std::string>& OtherBlock::getDirStore(){
	return (other_directives_);
}

/**
 * @brief location block에서는 limit_except 블록만 올 수 있습니다.
 *
 * @param line  블록이름 { 식으로 들어옵니다.
 * @param input 열어둔 파일시스템
 * @param line_len_ conf파일 현재까지 읽은 길이
 *
 * @warning limit_except 에 올 수 있는 메서드는 임의롤 줄였습니다.
 * 추가적으로 deny all이외에 다른 인자는 받지 않습니다.
 */
void	OtherBlock::makeBlock(std::string line, std::ifstream& input, int& line_len_){
	size_t pos = line.find('{');
	block_name_ = line.substr(0, pos);
	trimSidesSpace(block_name_);
	// std::cout << "5. |" << block_name_ << "|"<< std::endl;
	if (pos != line.size() - 1 || block_name_ == "")
		throw(std::runtime_error("this is not block"));
	switch(checkBlockName(block_name_)){
		case HTTPBLOCK :throw(std::runtime_error("this is not GOOD HTTP block"));
		case SERVERBLOCK : throw(std::runtime_error("HERE IS OTHER_BLOCK!"));
			break;
		case LOCATIONBLOCK : throw(std::runtime_error("this is not LOC block"));
		case OTHERBLOCK : makeOtherBlock(input, line_len_);
			break;
	}
}

/**
 * @brief Other 블록을 만드는 함수
 *
 * @param input 열어둔 파일시스템
 * @param line_len_ conf파일 현재까지 읽은 길이
 * @note 재귀적으로 돌기 위한 템플릿 함수 parseUntilEnd를 호출한다.
 */
void	OtherBlock::makeOtherBlock(std::ifstream& input,int& line_len_){
	OtherBlock new_block;
	other_store_.push_back(new_block);
	parseUntilEnd(input, line_len_, other_store_[other_store_.size() - 1]);
}
