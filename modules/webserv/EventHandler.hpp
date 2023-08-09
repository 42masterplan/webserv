#ifndef EVENTHANDLER_HPP
# define EVENTHANDLER_HPP
# include "Kqueue.hpp"
# include "../UData/HttpResponse/HttpResponseHandler.hpp"
class EventHandler
{
public:
	static const int BUFF_SIZE = 5000;
	static EventHandler&	getInstance();
	~EventHandler();
	void  sockReadable(struct kevent *cur_event);
	void  sockWritable(struct kevent *cur_event);
  void  cgiReadable(struct kevent *cur_event);
  void  cgiTerminated(UData* udata);
	void	fileReadable(struct kevent *cur_event);
	void	fileWritable(struct kevent *cur_event);
	void  disconnectFd(struct kevent *cur_event);
private:
	EventHandler(); 
	char          buff_[BUFF_SIZE];
};

#endif
