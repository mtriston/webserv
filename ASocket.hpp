#include "Worker.hpp"

class ASocket
{
private:
	int socket_;
public:
	ASocket(int socket) : socket_(socket) {}
	virtual ~ASocket() {}
	int getSocket() { return socket_; }
	virtual int fillFdSet(fd_set *readfds, fd_set *writefds) = 0;
	virtual IStrategy *makeStrategy() = 0;
};
