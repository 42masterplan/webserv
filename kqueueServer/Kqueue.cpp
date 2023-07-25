#include "Kqueue.hpp"

/**
 * @brief kqueue를 만들면서 서버 소켓을 첫번째 이벤트로 등록하는 함수
 *
 * @param serv_sock 서버소켓의 fd
 */
void  Kqueue::KqueueStart(const int &serv_sock){
	kqueue_fd_ = kqueue();
	if (kqueue_fd_ == -1)
		throw(std::runtime_error("kqueue() ERROR!!"));
	ChangeEvent(serv_sock, EVFILT_READ, EV_ADD | EV_ENABLE, NULL);//kqueue에 서버소켓 readEvent 등록
}

/**
 * @brief 이벤트를 등록하는 함수
 *
 * @param ident 이벤트를 감시할 대상에 대한 fd
 * @param filter 이벤트 필터 플래그 EVFILT_READ,EVFILT_WRITE
 * @param flags EV_ADD,EV_ENABLE,EV_DISABLE,EV_DELETE,EV_ONESHOT
 * @param udata 일반적으로 NULL
 * @note flags : EV_ADD ,EV_ENABLE (이벤트를 추가/활성화)
 *  EV_DISABLE, EV_DELETE(이벤트 비활성화 삭제),
 *  EV_ONESHOT(설정된 이벤트를 한번만 알려준다)
 */
void Kqueue::ChangeEvent(int ident, int filter, int flags, void * udata){
	struct kevent tmp_event;
	EV_SET(&tmp_event , ident, filter, flags, 0, 0, udata );
	change_list_.push_back(tmp_event);
}

/**
 * @brief kqueue에서 이벤트를 감지하는 함수
 *
 * @param event_list 이벤트를 담을 배열 현재는 8개로 생각
 * @return int 감지된 이벤트의 갯수
 */
int  Kqueue::detectEvent(struct kevent *event_list){
	int n_event = kevent(kqueue_fd_, &change_list_[0], change_list_.size(),event_list, 8, NULL);
	if (n_event == -1)
		 throw(std::runtime_error("kevent() ERROR!!"));
	change_list_.clear(); // clear change_list for new changes
	return (n_event);
}

