#include "UData.hpp"

UData::UData(e_fd_type type): status_code_(-1),client_fd_(-1),write_size_(0){ fd_type_ = type; }

UData::~UData(){}
