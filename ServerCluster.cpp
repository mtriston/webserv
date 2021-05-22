//
// Created by mtriston on 26.04.2021.
//

#include "ServerCluster.hpp"
#include "ASocket.hpp"
#include "Server.hpp"
#include "Config.hpp"
#include "Worker.hpp"

void ServerCluster::setup(std::vector<Config> &configs) {
  std::vector<Config>::iterator b = configs.begin();
  std::vector<Config>::iterator e = configs.end();
  while (b != e) {
    Server *server = new Server(&(*b));
    if (server->run()) {
      sockets_.push_back(server);
    }
    ++b;
  }
  pthread_mutex_init(&socketLock_, 0);
  pthread_mutex_init(&selectLock_, 0);
  pthread_mutex_init(&worksLock_, 0);

  workers_.push_back(new Worker(this));
  workers_.push_back(new Worker(this));

  workers_[0]->start();
  workers_[1]->start();
}

void ServerCluster::run() {
  while (1) {
    pthread_mutex_lock(&selectLock_);
    fd_set readfds, writefds;
    FD_ZERO(&readfds);
    FD_ZERO(&writefds);
    int max_fd = -1;
    pthread_mutex_lock(&socketLock_);
    for (std::list<ASocket *>::iterator i = sockets_.begin(); i != sockets_.end(); ++i) {
      max_fd = std::max((*i)->fillFdSet(&readfds, &writefds), max_fd);
    }
    int res = select(max_fd + 1, &readfds, &writefds, 0, 0);
    if (res == -1) {
      std::cerr << "Select error" << std::endl; //TODO: ???
      continue;
    } else if (res == 0) {
      continue; // тайм аут
    }
    for (std::list<ASocket *>::iterator i = sockets_.begin(); i != sockets_.end(); ++i) {
      if ((*i)->isReady(&readfds, &writefds)) {
        works_.push_back((*i)->makeWork());
      }
    }
    pthread_mutex_unlock(&socketLock_);
    pthread_mutex_unlock(&worksLock_);
  }
}

void ServerCluster::addSocket(ASocket *socket) {
  pthread_mutex_lock(&socketLock_);
  sockets_.push_back(socket);
  pthread_mutex_unlock(&socketLock_);
}

void ServerCluster::removeSocket(ASocket *socket) {
  pthread_mutex_lock(&socketLock_);
  sockets_.remove(socket);
  pthread_mutex_unlock(&socketLock_);
}

IWork *ServerCluster::getWork() {
  pthread_mutex_lock(&worksLock_);
  if (works_.empty()) {
    pthread_mutex_unlock(&selectLock_);
  }
  IWork *work = works_.front();
  works_.pop_front();
  pthread_mutex_unlock(&worksLock_);
  return work;
}

ServerCluster::ServerCluster() {}

ServerCluster::ServerCluster(ServerCluster const &) {}

ServerCluster &ServerCluster::operator=(ServerCluster const &) { return *this; }

ServerCluster::~ServerCluster() {}
