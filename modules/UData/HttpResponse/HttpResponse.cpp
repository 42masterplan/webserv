#include "HttpResponse.hpp"

HttpResponse::HttpResponse(){}

HttpResponse& HttpResponse::operator=(const HttpResponse &ref) {
	if (this == &ref)
		return *this;
	http_version_ = ref.http_version_;
	status_code_ = ref.status_code_;
	status_ = ref.status_;
	content_length_ = ref.content_length_;
	content_type_ = ref.content_type_;
	location_ = ref.location_;
	exist_session_ = ref.exist_session_;
	body_ = ref.body_;
	joined_data_ = ref.joined_data_;
	res_type_ = ref.res_type_;
	file_path_ = ref.file_path_;
	client_fd_ = ref.client_fd_;
	write_size_ = ref.write_size_;
	loc_block_ = ref.loc_block_;
	header_complete_ = ref.header_complete_;
	file_size_ = ref.file_size_;
	return *this;
}

HttpResponse::HttpResponse(HttpRequest &req) : http_version_("HTTP/1.1"), status_code_(200), status_(""), content_length_(0), content_type_(""), location_(""), exist_session_(req.getExistSession()), loc_block_((ConfParser::getInstance().getServBlock(req.getPort(), req.getHost())).findLocBlock(req.getPath())), res_type_(METHOD_TYPE), file_path_("") ,header_complete_(false){
  // std::cout << "REQUEST_HOST: " << req.getPath() << std::endl;
  // loc_block_.printInfo();
  // std::cout << "---------------------"<<std::endl;
  try{
		// loc_block_.printInfo();
		setFilePath(req, loc_block_);
		if (req.getMethod() == GET || req.getMethod() == HEAD)
			setFileSize(file_path_);
  }catch(std::exception& e){ //이곳은 isFolder에서 throw된 예외가 잡힙니다. 이 경우 존재하지 않는 폴더 혹은 파일의 요청입니다.
		std::cout << "Response 생성자에서 에러 발생!!" <<std::endl;
		processErrorRes(404);
  }
}

/* init */

void HttpResponse::processErrorRes(int status_code) {
	status_code_ = status_code;
	res_type_ = ERROR;
	file_path_ = getErrorPath(status_code);
}

void HttpResponse::processRedirectRes(int status_code) {
	status_code_ = status_code;

	status_ = status_msg_store_.getStatusMsg(status_code_);
	// status_ = status_store_[status_code_];
	
	std::string header =
	http_version_ + " " + status_ + "\r\n" +
	"Location: " + location_ + "\r\n\r\n";
	
	joined_data_.clear();
	joined_data_.insert(joined_data_.end(), header.begin(), header.end());
	// std::cout << std::string(joined_data_.begin(), joined_data_.end()) << "\n";
}
/**
 * @brief CGI response를 만드는 함수
 * joined_data에 정제되지 않은 데이터가 들어오고, body에는 cgi가 반환 한 body가 들어옵니다.
 * 
 * @return true 에러가 발생하지 않음
 * @return false 에러가 발생함
 */
bool	HttpResponse::makeCgiResponse(){
	std::cout << "CGI 리스폰스 만들기!"<<std::endl;
	if (joined_data_.size() < 8){
		std::cout << "여기서 나냐?" <<std::endl;
		print_vec(joined_data_);
		return (false);
	}
	std::string s(joined_data_.begin(), joined_data_.begin() + 8);
	if (s != "Status: ")
		return false;
	// std::cout << "CGI STRING" <<std::endl;
	// print_vec(joined_data_);
	joined_data_.erase(joined_data_.begin(),joined_data_.begin() + 8);
	std::string http_version  = http_version_ + " ";
	joined_data_.insert(joined_data_.begin(), http_version.begin(), http_version.end());
	std::string tmp = "\r\nContent-Length: " + std::to_string(body_.size());
	joined_data_.insert(joined_data_.end() - 4, tmp.begin(), tmp.end());
	// print_vec(joined_data_);
	// print_vec(body_);
	return true;
}


void	HttpResponse::makeBodyResponse(const HttpRequest& req){//TODO: status_code와 content_length를 바꾸기
	std::string	header = "";

	status_ = status_msg_store_.getStatusMsg(status_code_);
	// std::cout << "여기 왔다~~"<<std::endl;

	header += http_version_ + " " + status_ + "\r\n";

	// TODO: MimeStore 사용하기
	if (status_code_ == 201 || (status_code_ >= 300 && status_code_ < 400))
		header += "Location: " + location_ + "\r\n";
	if (status_code_ == 405 && loc_block_.getDenyMethod().size()) {
		std::vector<std::string> allow_method = loc_block_.getDenyMethod();
		header += "Allow: ";
		for (size_t i = 0; i < allow_method.size(); i++) {
			header += allow_method[i];
			if (i != allow_method.size() - 1)
				header += ", ";
		}
		header += "\r\n";
	}
	if ((status_code_ >= 200 && status_code_ < 400) && !exist_session_)
		header += "Set-Cookie: SESSIONID=" + Session::getInstance().createSession() + "\r\n";
	if (req.getContentType() != "")
		header += "Content-Type: " + req.getHeader().at("content-type") + "\r\n";
	else
		header += "Content-Type: text/html; charset=utf-8\r\n";
	header += "Content-Length: " + std::to_string(content_length_) + "\r\n\r\n";
	
	joined_data_.clear();
	joined_data_.insert(joined_data_.end(), header.begin(), header.end());
}


std::string HttpResponse::getErrorPath(int status_code){
	std::vector<int> error_codes = loc_block_.getErrorCode();
	std::vector<int>::iterator it = std::find(error_codes.begin(), error_codes.end(), status_code);	
	if (it != error_codes.end())
		return loc_block_.getCombineErrorPath();
	return std::string("");
}

/* getter, setter */
void HttpResponse::setStatusCode(int status_code) { status_code_ = status_code; }
void HttpResponse::setContentLength(int content_length) { content_length_ = content_length; }
std::vector<char> &HttpResponse::getBody() { return body_; }
const std::string &HttpResponse::getFilePath() const { return file_path_; }

void  HttpResponse::setFileSize(const std::string& file_path) {
  struct stat file_stat;
  if (file_path == ""){
    file_size_ = -1;
    return;
  }
  if (stat(file_path.c_str(), &file_stat) != 0)
    throw std::runtime_error("stat() ERROR");
	// std::cout << "파일 크기 만드는중 ~~" << file_stat.st_size <<std::endl;
  file_size_ = static_cast<long>(file_stat.st_size);
}

static bool isUploadMethod(HttpRequest &req) {
	const e_method method = req.getMethod();
	if (method == POST || method == PUT || method == PATCH)
		return true;
	return false;
}

void HttpResponse::setFilePath(HttpRequest &req, LocBlock &loc) {
  // loc.printInfo();
	file_path_ = loc.getReturnPath();
	if (file_path_ != "") {
		res_type_ = REDIRECT;
		location_ = file_path_;
		return; // 4 분기문 전부 processRes 여기서 하거나 밖에서 하거나 통일 좀 해야겠다
	}
	file_path_ = loc.getCombineCgiPath();
  // std::cout << "COMBINECGIPATH:" << file_path_ << "|" << std::endl;
	if (file_path_ != ""){
		res_type_ = CGI_EXEC;
		return;
	}
	file_path_ = loc.getCombineUploadStorePath();
	if (isUploadMethod(req)){//upload 하려고 하는데 그 경로가 설정파일에서 없으면 서버에러가 아니고 잘못된 요청
		if (file_path_ == ""){
			res_type_ = ERROR;
			processErrorRes(404);
			return ;
		}
		file_path_ +=  loc.getFileName();
		return;
	}
	file_path_ = loc.getCombineLocPath();
	// std::cout <<"file -------"<< req.getPath()<< "|" << file_path_ << std::endl;
  if (loc.isAutoIndex()){
		struct stat path_info;
		
    if (stat(loc.getCombineLocPath().c_str(), &path_info) != 0)
			throw(std::runtime_error("STAT ERROR()"));
		if (isFolder(loc.getCombineLocPath()) == true)
			res_type_ = AUTOINDEX;
    return;
  }
}

void	HttpResponse::setLocation(std::string location) { location_ = location; }


std::vector<char>& HttpResponse::getJoinedData(){ return joined_data_; }

// int main() {
// 	HttpResponse res;
// 	res.initStatusStore();
// 	res.processDefaultErrorRes(res, 404);
// 	res.processRedirectRes(res, 301);
// 	return 0;
// }
