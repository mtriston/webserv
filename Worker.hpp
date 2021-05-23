#ifndef WORKER_HPP
#define WORKER_HPP

class IWork;
class Cluster;
#include <pthread.h>

class Worker
{
 private:
  IWork *work_;
  Cluster *cluster_;
  pthread_t pthread_;
  bool keepOn;
  static void *_curcle(void *);
  void _setWork(IWork *work);
 public:
	explicit Worker(Cluster *cluster);
	~Worker();
	bool isKeepOn() const;
	void run();
	void finish();
	void getWork();
	void doWork();
};

#endif
