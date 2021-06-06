//
// Created by mtriston on 30.05.2021.
//

#include "WorkerManager.hpp"

WorkerManager::WorkerManager(int n, ISocketList *socketList, IWorkList *workList)
		: socketMutex_(), workMutex_(), downtimeMutex_(), activeWorkers_(0)
{
	socketList_ = socketList;
	workList_ = workList;
	pthread_mutex_init(&socketMutex_, 0);
	pthread_mutex_init(&workMutex_, 0);
	pthread_mutex_init(&downtimeMutex_, 0);
	for (int i = 0; i < n; ++i) {
		Worker *worker = new Worker(this);
		worker->run();
		workers_.push_back(worker);
	}
}

WorkerManager::~WorkerManager()
{
	for (size_t i = 0; i < workers_.size(); ++i) {
		delete workers_[i];
	}
	pthread_mutex_destroy(&socketMutex_);
	pthread_mutex_destroy(&workMutex_);
	pthread_mutex_destroy(&downtimeMutex_);
}

IWork *WorkerManager::getWork()
{
	while (true) {
		pthread_mutex_lock(&workMutex_);
		if (!workList_->isEmpty()) break;
		if (activeWorkers_ != 0) {
			pthread_mutex_unlock(&workMutex_);
		} else {
			pthread_mutex_unlock(&downtimeMutex_);
		}
	}
	IWork *work = workList_->getWork();
	pthread_mutex_unlock(&workMutex_);
	return work;
}

void WorkerManager::addSocket(ASocket *socket)
{
	pthread_mutex_lock(&socketMutex_);
	socketList_->addSocket(socket);
	pthread_mutex_unlock(&socketMutex_);
}

void WorkerManager::removeSocket(ASocket *socket)
{
	pthread_mutex_lock(&socketMutex_);
	socketList_->removeSocket(socket);
	pthread_mutex_unlock(&socketMutex_);
}

void WorkerManager::lockSocket()
{
	pthread_mutex_lock(&socketMutex_);
}

void WorkerManager::lockWork()
{
	pthread_mutex_lock(&workMutex_);
}

void WorkerManager::unlockSocket()
{
	pthread_mutex_unlock(&socketMutex_);
}

void WorkerManager::unlockWork()
{
	pthread_mutex_unlock(&workMutex_);
}

void WorkerManager::lockDowntime()
{
	pthread_mutex_lock(&downtimeMutex_);
}

void WorkerManager::unlockDowntime()
{
	pthread_mutex_unlock(&downtimeMutex_);

}
