#ifndef KQUEUE_HPP
# define KQUEUE_HPP
# include <sys/types.h>
# include <sys/event.h>
# include <sys/time.h>
# include <arpa/inet.h>
# include <unistd.h>
# include <string>
# include <iostream>
# include <vector>
# include <map>
# include <fcntl.h>
# include "UData.hpp"

/**
 * @brief kqueue에 이벤트를 등록하거나, 현재 이벤트가 발생했는지 감지하는 기능을 제공하는 정적 클래스입니다.
 */
class Kqueue{
	public:
		static void   kqueueStart(const std::vector<int>& serv_sock_fds);
		static void  ChangeEvent(const int& ident, int filter, int flags, void* udata);
		static int   detectEvent(struct kevent *event_list);
	private:
		~Kqueue();
		Kqueue();
		static int           kqueue_fd_;
		static std::vector<struct kevent>  change_list_; //등록할 이벤트를 담는 벡터. 담아주고 kqueue에 등록했다면, clear() 해서 비워줍니다.
};

#endif
