#ifndef EVENT_HANDLER_HPP
# define EVENT_HANDLER_HPP
# include "../Features/Kqueue.hpp"
# include "HttpResponseHandler.hpp"
class EventHandler
{
public:
	static const int BUFF_SIZE = 5000;

	~EventHandler();
	static EventHandler&	getInstance();
	void  sockReadable(struct kevent *event);
	void  sockWritable(struct kevent *event);
  void  cgiReadable(struct kevent *event);
  void  cgiWritable(struct kevent *event);
  void  cgiTimeout(struct kevent *event);
	void	fileReadable(struct kevent *event);
	void	fileWritable(struct kevent *event);
	void  disconnectFd(struct kevent *event);

private:
	char          buff_[BUFF_SIZE];

	EventHandler();
	bool	writeToclient(std::vector<char> &to_write, bool is_body, UData*	udata);

	void	fileErrorCallBack(struct kevent *event);
};

#endif
