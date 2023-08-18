#ifndef EVENTHANDLER_HPP
# define EVENTHANDLER_HPP
# include "../Features/Kqueue.hpp"
# include "HttpResponseHandler.hpp"
class EventHandler
{
public:
	static const int BUFF_SIZE = 5000;

	~EventHandler();
	static EventHandler&	getInstance();
	void  sockReadable(struct kevent *cur_event);
	void  sockWritable(struct kevent *cur_event);
  void  cgiReadable(struct kevent *cur_event);
  void  cgiWritable(struct kevent *cur_event);
  void  cgiTimeout(struct kevent *cur_event);
  // void  cgiTerminated(UData* udata);
	void	fileReadable(struct kevent *cur_event);
	void	fileWritable(struct kevent *cur_event);
	void  disconnectFd(struct kevent *cur_event);

private:
	char          buff_[BUFF_SIZE];

	EventHandler();
	bool	writeToclient(std::vector<char> &to_write, bool is_body, UData*	cur_udata);

	void	fileErrorCallBack(struct kevent *cur_event);
};

#endif
