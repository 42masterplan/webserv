#include "Kqueue.hpp"
#include <errno.h>

/* Non-usable: Kqueue is static class */
Kqueue::Kqueue() {}
Kqueue::~Kqueue() {}

/**
 * @brief kqueue를 만들고, 서버소켓의 read 이벤트를 등록하는 함수
 * @param serv_sock 서버소켓의 fd
 * @param change_list 서버소켓 read 이벤트를 저장할 kevent 벡터
 * @return 생성한 kqueue의 fd
 * @exception kqueue()함수에서 에러 발생 시 runtime_error를 throw합니다.
 */
int  Kqueue::KqueueStart(const int& serv_sock, std::vector<struct kevent>& change_list){
	int kqueue_fd = kqueue();
	if (kqueue_fd == -1)
		throw(std::runtime_error("kqueue() ERROR!!"));
	ChangeEvent(serv_sock, EVFILT_READ, EV_ADD | EV_ENABLE, NULL, change_list);//kqueue에 서버소켓 readEvent 등록

  return (kqueue_fd);
}

/**
 * @brief 등록할 이벤트를 생성하여 change_list_에 추가하는 함수
 * @param ident 이벤트를 감시할 대상에 대한 fd
 * @param filter 이벤트 필터 플래그 EVFILT_READ, EVFILT_WRITE
 * @param flags EV_ADD,EV_ENABLE,EV_DISABLE,EV_DELETE,EV_ONESHOT
 * @param udata 이벤트 핸들링에 필요한 모든 변수, 메소드가 담긴 클래스
 * @param change_list 생성한 이벤트를 추가할 kevent 벡터
 * @return EV_SET한 kevent구조체. event_list_에 추가해야 한다.
 * @note flags : EV_ADD ,EV_ENABLE (이벤트를 추가/활성화)
 *  EV_DISABLE, EV_DELETE(이벤트 비활성화 삭제),
 *  EV_ONESHOT(설정된 이벤트를 한번만 알려준다)
 */
void  Kqueue::ChangeEvent(int ident, int filter, int flags, void* udata, std::vector<struct kevent>& change_list){
	struct kevent tmp_event;
	EV_SET(&tmp_event, ident, filter, flags, 0, 0, udata);
  change_list.push_back(tmp_event);
}

/**
 * @brief kqueue에서 이벤트를 감지하는 함수
 * @param event_list 감지된 이벤트를 담을 8칸 배열
 * @param change_list kevent함수에 등록할 kevent 벡터. 등록 후 clear된다.
 * @param kqueue_fd kqueue의 fd
 * @return 감지된 이벤트의 갯수
 * @exception kevent()에서 에러 발생 시 runtime_error를 반환합니다.
 */
int  Kqueue::detectEvent(struct kevent *event_list, std::vector<struct kevent>& change_list, const int& kqueue_fd){
	int n_event = kevent(kqueue_fd, &change_list[0], change_list.size(), event_list, 8, NULL);
	if (n_event == -1){
    std::cerr << errno << "\n";
    throw(std::runtime_error("kevent() ERROR!!"));
  }
	change_list.clear(); //등록한 이벤트들은 삭제
	return (n_event);
}

