//
// Created by mtriston on 26.04.2021.
//

#include "Cluster.hpp"
#include "ASocket.hpp"
#include "ListenSocket.hpp"
#include "Config.hpp"
#include "Worker.hpp"

void Cluster::setup(std::vector<Config> &configs) {
    std::vector<Config>::iterator b = configs.begin();
    std::vector<Config>::iterator e = configs.end();
    while (b != e) {
        ListenSocket *server = new ListenSocket(&(*b));
        if (server->run()) {
            socketList_->addSocket(server);
        }
        ++b;
    }
}

void Cluster::run() {
    fd_set readfds, writefds;
    while (true) {
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
        int res = select(max_fd + 1, &readfds, &writefds, 0, 0);
        if (res == -1) {
            std::cerr << "Select error" << std::endl; //TODO: ???
            continue;
        } else if (res == 0) {
            continue;
        }
        for (size_t i = 0; i < sockets.size(); ++i) {
            if (sockets[i]->isReady(&readfds, &writefds)) {
                workList_->addWork(sockets[i]->getWork());
            }
        }
        workerManager_->unlockWork();
    }
}

Cluster::Cluster() {
    socketList_ = new SocketList();
    workList_ = new WorkList();
    workerManager_ = new WorkerManager(2, socketList_, workList_);
}

Cluster::Cluster(Cluster const &) {}

Cluster &Cluster::operator=(Cluster const &) { return *this; }

Cluster::~Cluster() {
    delete socketList_;
    delete workList_;
    delete workerManager_;
}
