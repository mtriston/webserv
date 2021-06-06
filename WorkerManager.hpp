//
// Created by mtriston on 30.05.2021.
//

#ifndef WEBSERV__WORKERMANAGER_HPP_
#define WEBSERV__WORKERMANAGER_HPP_

#include "Worker.hpp"
#include "ISocketList.hpp"
#include "IWorkList.hpp"

class WorkerManager {
private:
	ISocketList *socketList_;
	IWorkList *workList_;
	std::vector<Worker *> workers_;
	pthread_mutex_t socketMutex_;
	pthread_mutex_t workMutex_;
	pthread_mutex_t downtimeMutex_;

	WorkerManager(WorkerManager const &);

	WorkerManager &operator=(WorkerManager const &);

public:
	int activeWorkers_;

	IWork *getWork();

	void addSocket(ASocket *);

	void removeSocket(ASocket *);

	void lockSocket();

	void unlockSocket();

	void lockWork();

	void unlockWork();

	void lockDowntime();

	void unlockDowntime();

	WorkerManager(int, ISocketList *, IWorkList *);

	~WorkerManager();
};

#endif //WEBSERV__WORKERMANAGER_HPP_
