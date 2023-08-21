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
  try{
		setFilePath(req, loc_block_);
		if (res_type_ == REDIRECT)
			return ;
		if (req.getMethod() == GET || req.getMethod() == HEAD)
			setFileSize(file_path_);
  }catch(std::exception& e){ //이곳은 isFolder에서 throw된 예외가 잡힙니다. 이 경우 존재하지 않는 폴더 혹은 파일의 요청입니다.
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
	
	std::string header =
	http_version_ + " " + status_ + "\r\n" +
	"Location: " + location_ + "\r\n\r\n";
	
	joined_data_.clear();
	joined_data_.insert(joined_data_.end(), header.begin(), header.end());
}
/**
 * @brief CGI response를 만드는 함수
 * joined_data에 정제되지 않은 데이터가 들어오고, body에는 cgi가 반환 한 body가 들어옵니다.
 * 
 * @return true 에러가 발생하지 않음
 * @return false 에러가 발생함
 */
bool	HttpResponse::makeCgiResponse(){
	if (joined_data_.size() < 8)
		return false;
	std::string s(joined_data_.begin(), joined_data_.begin() + 8);
	if (s != "Status: ")
		return false;
	joined_data_.erase(joined_data_.begin(),joined_data_.begin() + 8);
	std::string http_version  = http_version_ + " ";
	joined_data_.insert(joined_data_.begin(), http_version.begin(), http_version.end());
	std::string tmp = "\r\nContent-Length: " + std::to_string(body_.size());
	joined_data_.insert(joined_data_.end() - 4, tmp.begin(), tmp.end());
	return true;
}


void	HttpResponse::makeBodyResponse(){
	std::string	header = "";
	const std::string& file_name_ref = loc_block_.getFileName();
	status_ = status_msg_store_.getStatusMsg(status_code_);

	header += http_version_ + " " + status_ + "\r\n";

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
	if (file_name_ref.find_last_of('.') != std::string::npos)
		header += "Content-Type: " + MimeStore::getMime(file_name_ref.substr(file_name_ref.find_last_of('.'))) + "; charset=utf-8\r\n";
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
  file_size_ = static_cast<long>(file_stat.st_size);
}

static bool isUploadMethod(HttpRequest &req) {
	const e_method method = req.getMethod();
	if (method == POST || method == PUT || method == PATCH)
		return true;
	return false;
}

void HttpResponse::setFilePath(HttpRequest &req, LocBlock &loc) {
	file_path_ = loc.getReturnPath();
	if (file_path_ != "") {
		res_type_ = REDIRECT;
		location_ = file_path_;
		return;
	}
	file_path_ = loc.getCombineCgiPath();
	if (file_path_ != ""){
		res_type_ = CGI_EXEC;
		return;
	}
	file_path_ = loc.getCombineUploadStorePath();
	if (isUploadMethod(req)){
		if (file_path_ == ""){
			res_type_ = ERROR;
			processErrorRes(404);
			return ;
		}
		file_path_ +=  loc.getFileName();
		return;
	}
	file_path_ = loc.getCombineLocPath();
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
