#ifndef ASOCKET_HPP
# define ASOCKET_HPP

#include <sys/types.h>

class Config;
class IWork;

class ASocket
{
protected:
	int socket_;
	Config *config_;
	ASocket();
public:
	ASocket(int socket, Config *config);
	virtual ~ASocket();
	int getSocket() const;
	Config *getConfig();
	virtual int fillFdSet(fd_set *readfds, fd_set *writefds) = 0;
	virtual bool isReady(fd_set *readfds, fd_set *writefds) = 0;
	virtual IWork *makeWork() = 0;
};

#endif
