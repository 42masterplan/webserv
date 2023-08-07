#include "UData.hpp"

UData::UData(e_fd_type type){ fd_type_ = type; }

UData::~UData(){}

void UData::generateResponses(void) {
	for (size_t i = 0; i < http_request_.size(); i++) {
		if (http_request_[i].getParseStatus() != FINISH)
			break;
		http_response_.push_back(HttpResponse(http_request_[i]));
	}
}