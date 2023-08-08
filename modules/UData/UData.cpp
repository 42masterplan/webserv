#include "UData.hpp"
# include "./HttpResponse/HttpResponse.hpp"
UData::UData(e_fd_type type, int port):fd_type_(type), client_fd_(-1), write_size_(0), port_(port) {}

UData::~UData(){}
UData(){}