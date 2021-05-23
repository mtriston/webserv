#ifndef ASOCKET_HPP
# define ASOCKET_HPP

#include <sys/types.h>
#include <pthread.h>

class Config;
class IWork;

class ASocket
{
protected:
	int socket_;
	Config *config_;
	pthread_mutex_t mutex_;
	volatile bool isBusy_;
	ASocket();
	ASocket(ASocket const &);
public:
	ASocket(int socket, Config *config);
	virtual ~ASocket();
	int getSocket() const;
	Config *getConfig();
	void lockMutex();
	void unlockMutex();
	bool isBusy() const;
	void setIsBusy(bool);
	virtual int fillFdSet(fd_set *readfds, fd_set *writefds) = 0;
	virtual bool isReady(fd_set *readfds, fd_set *writefds) = 0;
	virtual IWork *getWork() = 0;
};

#endif
