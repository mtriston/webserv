#ifndef ASOCKET_HPP
# define ASOCKET_HPP

#include <netinet/in.h>

#include "Config.hpp"
#include "Worker.hpp"
class IWork;
class Session;
class Server;
class ServerCluster;

class ASocket
{
protected:
	int socket_;
	Config *config_;
	ASocket();
public:
	ASocket(int socket, Config *config);
	virtual ~ASocket();
	int getSocket();
	Config *getConfig();
	virtual int fillFdSet(fd_set *readfds, fd_set *writefds) = 0;
	virtual bool isReady(fd_set *readfds, fd_set *writefds) = 0;
	virtual IWork *makeWork() = 0;
};

#endif
