#include "UData.hpp"
# include "./HttpResponse/HttpResponse.hpp"
UData::UData(e_fd_type type): client_fd_(-1), write_size_(0){ fd_type_ = type; }

UData::~UData(){}
