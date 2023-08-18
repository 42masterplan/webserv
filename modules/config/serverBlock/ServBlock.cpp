#include "ServBlock.hpp"

/**
 * @brief Construct a new Serv Block:: Serv Block object
 * default listen은 -1번으로 해서 에러로 처리할 예정입니다.
 */
ServBlock::ServBlock() : upload_store_(""), listen_(-1){}

ServBlock::~ServBlock(){}

/*getter*/
const std::string& ServBlock::getUploadStore()const{return upload_store_;}
const std::vector<std::string>& ServBlock::getServerName()const{return server_name_;}
const int& ServBlock::getListen()const{return listen_;}

/**
 * @brief 실제 사용할 경로를 찾아줄 getter
 *
 * @return std::string 실제로 사용할 upload_store 경로
 */
std::string ServBlock::getCombineUploadStorePath()const{
	if (upload_store_ == "")
		return (upload_store_);
	return(root_ + upload_store_);
}

/**
 * @brief 모든 server block의 데이터를 사용가능하도록 map에 담긴걸 클래스 멤버에 사용하기 좋은 값으로 담아주는 함수
 */
void	ServBlock::refineAll(){
	parseHttpDirective(serv_directives_);
	parseServDirective();
	if (loc_store_.size() == 0)
		throw(std::runtime_error("You must input Locaction block least One Block!"));
	for (size_t i = 0; i < loc_store_.size(); i++){
    loc_store_[i].setInherit(*this);//TODO: 확인필요
		loc_store_[i].refineAll();
	}
	std::sort(loc_store_.begin(), loc_store_.end(), cmp);
}

/**
 * @brief 전달된 경로에 대해서 어떤 규칙을 따라야 하는지 리턴해주는 함수입니다.
 *
 * @param http에서 path를 추출해서 원하는 블럭을 얻을 때 사용합니다.
 * @return LocBlock 전딜힌 path에 대해서 해당하는 location block을 리턴해줍니다. 만약 location블록을 찾을 수 없다면 location블록의 loc_info_와 combined_path_에 "" 가 들어간게 리턴됩니다.
 * @warning heap buffer overflow 때문에 locblock을 깊은 복사하지 않고 얕은 복사를 진행합니다.
 * 따라서 여기에 있는 vector나 map을 수정하면 원본 locblock에도 영향이 간다는 것을 알고 진행합시다.
 */
LocBlock ServBlock::findLocBlock(std::string path){
	const std::vector<std::string>& serv_index_store = getIndex();
	int ret = -1;
	for (size_t i = 0;i < serv_index_store.size(); i++){
		ret = untilFindLoc(path, root_, serv_index_store[i]);
		if (ret != -1){
			std::string con_p = loc_store_[ret].getCombineLocPath();
			if (con_p.size() != 1 &&con_p.back() == '/'){
				if (isFolder(con_p) == false)
					con_p.pop_back();
				loc_store_[ret].setCombinePath(con_p);
			}
			// loc_store_[ret].printInfo();
			return (loc_store_[ret]);
		}
	}
	LocBlock ret_loc("");
	ret_loc.setInherit(*this);
	ret_loc.setError();
	return ret_loc;
}

/**
 * @brief serverBlock의 모든 정보를 출력하면서 Server block이 담고있는 모든 location Block의 정보까지 출력하는 함수
 *
 */
void	ServBlock::printInfo()const{
	printHttpInfo();
	std::cout << "\n---------------[Server]------------------\n";
	std::cout << "upload_store_:|" << upload_store_ << "|\n";
	std::cout << "[server_name_]\n";
	for (size_t i = 0;i < server_name_.size(); i++){
			std::cout << "server_name_[" << i << "]:|" << server_name_[i] << "|\n";
	}
	std::cout << "listen:|" << listen_ << "|\n";
	std::cout << "\n---------------[!!!!!INTO ALL Location Block!!!!!]------------------\n";
	for (size_t i = 0; i < loc_store_.size(); i++){
		loc_store_[i].printInfo();
	}
}

/**
 * @brief 에러 블럭인지 아닌지 확인하는 함수
 *
 * @return true 에러블럭이다.
 * @return false 에러블럭이 아니다.
 */
	bool ServBlock::isErrorBlock()const{
		if (listen_ == -1)
			return (true);
		return false;
	}

/* 사용자가 호출하지 않는 public 함수 (재귀 템플릿 때문에 public.) */
/**
 * @brief 블록을 만드는 함수 LocBlock 블록과 otherBlock허용
 *
 * @param line 파싱중인 한줄 라인(주석과 스페이스를 삭제한)
 * @param input 파싱중인 설정파일
 * @param line_len_ 설정파일을 어디까지 읽었는지 확인
 * @note 템플릿 사용을 위해 반드시 필요
 * @warning  http, SERVER 블록은 에러로 감지함
 */
void	ServBlock::makeBlock(std::string line, std::ifstream& input, int& line_len_){
	size_t pos = line.find('{');
	std::string block_name = line.substr(0, pos);
	trimSidesSpace(block_name);
	// std::cout << "5. |" << block_name << "|"<< std::endl;
	if (pos != line.size() - 1 || block_name == "")
		throw(std::runtime_error("this is not block"));
	switch(checkBlockName(block_name)){
		case HTTPBLOCK :throw(std::runtime_error("this is not GOOD HTTP block"));
		case SERVERBLOCK : throw(std::runtime_error("this is not GOOD SERVER block"));
		case LOCATIONBLOCK : makeLocBlock(input, line_len_, block_name);
			break;
		case OTHERBLOCK : makeOtherBlock(input, line_len_);
			break;
	}
}

/**
 * @brief serv_directives_ 반환
 * @note 템플릿 사용 위해서 반드시 필요
 * @return std::map<std::string, std::string>& 인자로 받아서 변경시키기 위해서 레퍼런스 타입으로 반환함
 */
std::map<std::string, std::string>& ServBlock::getDirStore(){return (serv_directives_);}

/*private*/
/**
 * @brief 로케이션 block을 찾는 함수입니다.
 *
 * @param path http path를 의미합니다.
 * @param root root가 있는 경우 root를 찾습니다.
 * @param index index를 붙혀서 같이 찾습니다.
 * @return int 몇번 째 블록을 보면 되는지 확인합니다.
 */
int ServBlock::untilFindLoc(const std::string& path, const std::string& root, const std::string& index){
	std::string con_path = path;
	if (path.back() != '/')
		con_path = con_path + "/" + index;
	else 
		con_path = con_path + index;
	for (size_t i = 0; i < loc_store_.size(); i++){//모든 로케이션 블록을 돌면서 탐색
		std::string troot = root;
		if (loc_store_[i].getRoot() != "")//현재 로케이션 블럭의 Root를 가져온다.
			troot = loc_store_[i].getRoot();
		std::string loc_info = loc_store_[i].getLocInfo();//로케이션 블록의 Loc인포를 가져온다.
		if (loc_info.find("/.") != std::string::npos){//와일드 카드를 처리하기 위해서 /.으로 끝나는 경우 찾아준다.
			for(int j = con_path.size() - 1; j >= 0; j--){//합성 경로에서 뒤에서부터 탐색해서 .을 찾는다.
				if (con_path[j] == '.'){
					std::cout << "|"<< con_path.substr(j + 1) <<"|" <<  loc_info.substr(loc_info.find("/.") + 2) + "/"<< "|\n";
					if (con_path.substr(j + 1) == (loc_info.substr(loc_info.find("/.") + 2) + "/")){//합성 경로의 뒷부분의 .bla와 location 블록의 .bla가 일치하는 경우 그것을 리턴한다.
						//와일드 카드 안에 index가 있는 건 처리하지 않았다.
            // std::cout << "찾았다!!" <<std::endl;
						loc_store_[i].setCombinePath(troot + path + index);
						loc_store_[i].setHighPriorityRoot(troot);
						return (i);
					}
					break;//만약 .으로 끝났는데 와일드 카드가 아니면 일단 break;
				}
			}
		}
		if (con_path.find(loc_info) == 0){//합성 경로에서 현재 location이 있다면
			const std::vector<std::string>& loc_index_store = loc_store_[i].getIndex();
			std::string left_path = con_path.substr(loc_info.size());//이게 파일 경로를 뺀 url에서 해당부분을 뺀 나머지 부분이 된다.
			if (left_path.find("/")!= 0)
						left_path = "/" + left_path;
			// std::cout << "@@@@left_path:" << left_path<< "|" << path <<std::endl;
			
			if (left_path == "")
				left_path = "/";
			if (loc_index_store.size()  == 0 || (loc_index_store.size() == 1 && loc_index_store[0]  == ""))//인덱스가 없는 경우 바로 이 블록이 해당하는 블록이다.
				return (locBlockSetUp(left_path,i , troot));
			else {//인덱스가 있다면!
				for (size_t j = 0; j < loc_index_store.size(); j++){//모든 인덱스를 순회하면서
					// std::cout <<"남은 경로에요~" <<left_path << "|" << path <<std::endl;
					if (left_path == ("/" + loc_index_store[j] + "/")) //left path와 인덱스가 같다면 인덱스를 어차피 붙혀줘서 탐색을 하기 때문에 따로 하지 않아도 됩니다.
						left_path = "/";
					if (path.back() != '/'){
						// std::cout << "1번이에용" << troot + path + "/" + loc_index_store[j] <<std::endl;
						std::string open_path = (troot + left_path); //파일을 열어보는 경로이다.
						open_path.pop_back();
						if (!isFolder(open_path)){//폴더가 아니면서
							  struct stat path_info;
							  if (stat(open_path.c_str(), &path_info) == 0){//존재하는 파일인 경우만
									loc_store_[i].setCombinePath(open_path);
									return (i);
								}
						}
						loc_store_[i].setCombinePath(troot + path + "/" + loc_index_store[j]);
					}
					else {
						// std::cout << "2번이에용" << troot + path + "/" + loc_index_store[j] <<std::endl;
						loc_store_[i].setCombinePath(troot + path + loc_index_store[j]);
					}
					int ret = untilFindLoc(left_path, troot, loc_index_store[j]);
					if (ret != -1)
						return(ret);
				}//인덱스를 다 탐색했는데..! 없다?
				return (locBlockSetUp(left_path,i , troot));
			}
		}
	}
	return (-1);
}


int ServBlock::locBlockSetUp(std::string &left_path, int ret_idx, std::string & root){
	loc_store_[ret_idx].setCombinePath(root + left_path);
	if (left_path.back() == '/')
		left_path.pop_back();
	loc_store_[ret_idx].setFileName(left_path);
	loc_store_[ret_idx].setHighPriorityRoot(root);
	return ret_idx;
}
/**
 * @brief 로케이션 블록을 만드는 함수
 *
 * @param input 열어둔 설정파일
 * @param line_len_ 설정파일 읽은 길이
 * @note 재귀적으로 돌기 위한 템플릿 함수 parseUntilEnd를 호출한다.
 */
void	ServBlock::makeLocBlock(std::ifstream& input, int& line_len_, std::string& block_name){
	if (block_name == "location")
		throw(std::runtime_error("location block Don't have pathinfo"));
	std::string loc_info =  block_name.substr(8);
	trimSidesSpace(loc_info);
	LocBlock new_block(loc_info);
	loc_store_.push_back(new_block);
	parseUntilEnd(input, line_len_, loc_store_[loc_store_.size() - 1]);
}

/**
 * @brief Other 블록을 만드는 함수
 *
 * @param input 열어둔 설정파일
 * @param line_len_ 설정파일 읽은 길이
 * @note 재귀적으로 돌기 위한 템플릿 함수 parseUntilEnd를 호출한다.
 */
void	ServBlock::makeOtherBlock(std::ifstream& input, int& line_len_){
	OtherBlock new_block;
	other_store_.push_back(new_block);
	parseUntilEnd(input, line_len_, other_store_[other_store_.size() - 1]);
}

/**
 * @brief 서버에만 해당하는 데이터를 멤버변수에 담아주는 함수
 * @details server_name이 설정 안되는 경우가 존재할 수 있다.
 * [subject] : Setup the server_names or not.
 */
void	ServBlock::parseServDirective(){
	std::map<std::string, std::string>::iterator it = serv_directives_.find("server_name");
	if (it == serv_directives_.end())
		server_name_.push_back("");
	splitAndStore(server_name_, (*it).second, ' ');
	it = serv_directives_.find("listen");//default_server안 받겠습니다.
	if (it == serv_directives_.end())
		throw(std::runtime_error("You must at least one listen!!!"));
	listen_ = stringToInt((*it).second);
	it = serv_directives_.find("upload_store");
	if (it != serv_directives_.end())
		upload_store_ = (*it).second;
}

