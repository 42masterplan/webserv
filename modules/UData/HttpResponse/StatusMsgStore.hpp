#ifndef STATUS_STORE_HPP
#define STATUS_STORE_HPP

#include <map>
#include <string>

class StatusMsgStore {
	public:
		StatusMsgStore();

		void initStatusMsgStore();
		std::string getStatusMsg(int status_msg_code);

		static std::map<int, std::string> status_msg_store_;
};

#endif
