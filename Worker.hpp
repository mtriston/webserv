#ifndef WORKER_HPP
#define WORKER_HPP

class IWork;
class ServerCluster;
#include <pthread.h>

class Worker
{
 public:
	IWork *work_;
	ServerCluster *cluster_;
    pthread_t *pthread_;
	Worker(ServerCluster *cluster);
	~Worker();
	void start();
	void setWork(IWork *work);
	static void *doWork(void *self);

};

#endif