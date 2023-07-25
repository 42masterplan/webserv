#ifndef KQUEUE_HPP
# define KQUEUE_HPP
#include <sys/types.h>
#include <sys/event.h>
#include <sys/time.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string>
#include <iostream>
#include <vector>
#include <map>
#include <fcntl.h>
/**
 * @brief kqueue에 이벤트를 변경하거나 현재 이벤트가 발생했는지 감지할 수 있습니다.
 */
class Kqueue{
public:
	Kqueue();
	~Kqueue();
	void  KqueueStart(const int &serv_sock);
	void ChangeEvent(int ident, int filter, int flags, void *udata);
	int  detectEvent(struct kevent *event_list);
private:
	std::vector<struct kevent> change_list_; //등록할 이벤트만 담아주고 kqueue에 등록했다면, clear() 해서 비워줍니다.
	int kqueue_fd_;
};

#endif
