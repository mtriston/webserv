//
// Created by mtriston on 26.04.2021.
//

#include "Cluster.hpp"
#include "Config_parser.hpp"
#include "WorkerManager.hpp"
#include "WorkList.hpp"
#include "SocketList.hpp"
#include "ASocket.hpp"
#include "ListenSocket.hpp"

bool Cluster::setup(std::string const &configFile)
{
	int count = 0;

	if (!configParser_->init(configFile.c_str())) {
		return false;
	}
	std::vector<std::pair<std::string, int> > allListen = configParser_->getAllListen();
	for (size_t i = 0; i < allListen.size(); ++i) {
		ListenSocket *s = new ListenSocket(allListen[i].first, allListen[i].second, configParser_);
		if (s->run()) {
			++count;
			socketList_->addListenSocket(s);
		} else {
			delete s;
		}
	}
	return count != 0;
}

void Cluster::run()
{
	fd_set readfds, writefds;
	while (true) {
		timeval time = {10, 0};
		workerManager_->lockDowntime();
		workerManager_->lockSocket();
		std::vector<ASocket *> sockets = socketList_->getSocketArray();
		workerManager_->unlockSocket();
		FD_ZERO(&readfds);
		FD_ZERO(&writefds);
		int max_fd = -1;
		for (size_t i = 0; i < sockets.size(); ++i) {
			max_fd = std::max(sockets[i]->fillFdSet(&readfds, &writefds), max_fd);
		}
		int res = select(max_fd + 1, &readfds, &writefds, 0, &time);
		if (res <= 0) {
			socketList_->resetAllConnections();
		} else {
			for (size_t i = 0; i < sockets.size(); ++i) {
				if (sockets[i]->isReady(&readfds, &writefds)) {
					workList_->addWork(sockets[i]->getWork());
				}
			}
		}
		workerManager_->unlockWork();
	}
}

Cluster::Cluster()
{
	socketList_ = new SocketList();
	workList_ = new WorkList();
	configParser_ = new Config_parser();
	workerManager_ = new WorkerManager(1, socketList_, workList_);
}

Cluster::Cluster(Cluster const &) {}

Cluster &Cluster::operator=(Cluster const &) { return *this; }

Cluster::~Cluster()
{
	delete socketList_;
	delete workList_;
	delete workerManager_;
	delete configParser_;
}