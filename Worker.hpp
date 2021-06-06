#ifndef WORKER_HPP
#define WORKER_HPP

class IWork;

class WorkerManager;

#include <pthread.h>

class Worker {
private:
	IWork *work_;
	WorkerManager *workerManager_;
	pthread_t pthread_;
	bool keepOn;

	static void *_curcle(void *);

	void _setWork(IWork *work);

public:
	explicit Worker(WorkerManager *);

	~Worker();

	bool isKeepOn() const;

	void run();

	void getWork();

	void doWork();
};

#endif
