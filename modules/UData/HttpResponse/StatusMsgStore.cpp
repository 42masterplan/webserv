#include "StatusMsgStore.hpp"
#include <iostream>


StatusMsgStore::StatusMsgStore() {
	this->initStatusMsgStore();
}

std::map<int, std::string> StatusMsgStore::status_msg_store_;

void StatusMsgStore::initStatusMsgStore() {
	status_msg_store_[200] = "200 OK";
	status_msg_store_[201] = "201 Created";
	status_msg_store_[202] = "202 Accepted";
	status_msg_store_[204] = "204 No Content";
	status_msg_store_[301] = "301 Moved Permanently";
	status_msg_store_[302] = "302 Found";
	status_msg_store_[304] = "304 Not Modified";
	status_msg_store_[400] = "400 Bad Request";
	status_msg_store_[401] = "401 Unauthorized";
	status_msg_store_[403] = "403 Forbidden";
	status_msg_store_[404] = "404 Not Found";
	status_msg_store_[405] = "405 Method Not Allowed";
	status_msg_store_[500] = "500 Internal Server Error";
	status_msg_store_[501] = "501 Not Implemented";
	status_msg_store_[502] = "502 Bad Gateway";
	status_msg_store_[503] = "503 Service Unavailable";
}

std::string StatusMsgStore::getStatusMsg(int status_msg_code) {
	return status_msg_store_[status_msg_code];
}
