#include "Worker.hpp"

#include "IWork.hpp"
#include "ServerCluster.hpp"

Worker::Worker(ServerCluster *cluster){
  work_ = 0;
  cluster_ = cluster;
  pthread_ = new pthread_t();
};

Worker::~Worker() { delete this->work_; }

void Worker::setWork(IWork *work) {
	delete work_;
	work_ = work;
}

void *Worker::doWork(void *self) {
	while (1) {
	  std::cout << "Hello" << std::endl;
		static_cast<Worker*>(self)->setWork(static_cast<Worker*>(self)->cluster_->getWork());
      static_cast<Worker*>(self)->work_->doWork(static_cast<Worker*>(self)->cluster_);
	}
	return 0;
}

void Worker::start() {
  ::pthread_create(pthread_, 0, doWork, this);
}
