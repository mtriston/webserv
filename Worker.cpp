#include "Worker.hpp"

#include "WorkerManager.hpp"
#include "IWork.hpp"

Worker::Worker(WorkerManager *workerManager)
{
	work_ = 0;
	workerManager_ = workerManager;
	keepOn = true;
}

Worker::~Worker()
{
	delete this->work_;
	keepOn = false;
	::pthread_join(pthread_, 0);
}

void Worker::_setWork(IWork *work)
{
	delete work_;
	work_ = work;
}

void Worker::getWork()
{
	_setWork(workerManager_->getWork());
}

void Worker::doWork()
{
	work_->doWork(workerManager_);
}

void Worker::run()
{
	::pthread_create(&pthread_, 0, _curcle, this);
}

void *Worker::_curcle(void *arg)
{
	Worker *self = static_cast<Worker *>(arg);
	while (self->isKeepOn()) {
		self->getWork();
		self->workerManager_->activeWorkers_++;
		self->doWork();
		self->workerManager_->activeWorkers_--;
	}
	return 0;
}

bool Worker::isKeepOn() const
{
	return keepOn;
}
