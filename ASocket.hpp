#include "Worker.hpp"

class ASocket
{
protected:
	int socket_;
	Config *config_;
	ASocket();
public:
	ASocket(int socket, Config *config) : socket_(socket), config_(config) {}
	virtual ~ASocket() {}
	int getSocket() { return socket_; }
	Config *getConfig() { return config_; }
	virtual int fillFdSet(fd_set *readfds, fd_set *writefds) = 0;
	virtual bool isReady(fd_set *readfds, fd_set *writefds) = 0;
	virtual IWork *makeWork() = 0;
};
