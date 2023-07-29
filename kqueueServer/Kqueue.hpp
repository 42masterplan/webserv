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

/**
 * @brief kqueue에 이벤트를 등록하거나, 현재 이벤트가 발생했는지 감지하는 기능을 제공하는 정적 클래스입니다.
 */
class Kqueue{
  public:
    static int   KqueueStart(const int& serv_sock, std::vector<struct kevent>& change_list);
    static void  ChangeEvent(int ident, int filter, int flags, void* udata, std::vector<struct kevent>& change_list);
    static int   detectEvent(struct kevent *event_list, std::vector<struct kevent>& change_list, const int& kqueue_fd);
  private:
    ~Kqueue();
    Kqueue();
};

#endif
