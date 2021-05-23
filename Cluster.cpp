//
// Created by mtriston on 26.04.2021.
//

#include "Cluster.hpp"
#include "ASocket.hpp"
#include "ListenSocket.hpp"
#include "Config.hpp"
#include "Worker.hpp"
#include "IWork.hpp"

void Cluster::setup(std::vector<Config> &configs) {
  std::vector<Config>::iterator b = configs.begin();
  std::vector<Config>::iterator e = configs.end();
  while (b != e) {
    ListenSocket *server = new ListenSocket(&(*b));
    if (server->run()) {
      sockets_.push_back(server);
    }
    ++b;
  }
  pthread_mutex_init(&socketMutex_, 0);
  pthread_mutex_init(&selectMutex_, 0);
  pthread_mutex_init(&worksMutex_, 0);
  pthread_mutex_init(&activeWorkersMutex_, 0);
  pthread_mutex_lock(&selectMutex_);

  for (int i = 0; i < COUNT_OF_WORKERS; ++i) {
    Worker *worker = new Worker(this);
    worker->run();
    workers_.push_back(worker);
  }
}

void Cluster::run() {
  while (true) {
    pthread_mutex_lock(&selectMutex_);
    pthread_mutex_lock(&socketMutex_);
    fd_set readfds, writefds;
    FD_ZERO(&readfds);
    FD_ZERO(&writefds);
    int max_fd = -1;
    for (std::list<ASocket *>::iterator i = sockets_.begin(); i != sockets_.end(); ++i) {
      max_fd = std::max((*i)->fillFdSet(&readfds, &writefds), max_fd);
    }
    int res = select(max_fd + 1, &readfds, &writefds, 0, 0);
    if (res == -1) {
      std::cerr << "Select error" << std::endl; //TODO: ???
      continue;
    } else if (res == 0) {
      std::cout << "Select timeout" << std::endl; //TODO: ???
      continue;
    }
    for (std::list<ASocket *>::iterator i = sockets_.begin(); i != sockets_.end(); ++i) {
      if ((*i)->isReady(&readfds, &writefds)) {
        works_.push_back((*i)->getWork());
      }
    }
    pthread_mutex_unlock(&socketMutex_);
    pthread_mutex_unlock(&worksMutex_);
  }
}

void Cluster::addSocket(ASocket *socket) {
  pthread_mutex_lock(&socketMutex_);
  sockets_.push_back(socket);
  pthread_mutex_unlock(&socketMutex_);
}

void Cluster::removeSocket(ASocket *socket) {
  pthread_mutex_lock(&socketMutex_);
  sockets_.remove(socket);
  delete socket;
  pthread_mutex_unlock(&socketMutex_);
}

IWork *Cluster::getWork() {
  while(true) {
    pthread_mutex_lock(&worksMutex_);
    if (!works_.empty()) break;
    if (activeWorkers_ == 0) {
      pthread_mutex_unlock(&selectMutex_);
    } else {
      pthread_mutex_unlock(&worksMutex_);
      continue;
    }
  }
    IWork *work = works_.front();
    works_.pop_front();
    pthread_mutex_unlock(&worksMutex_);
    return work;
}

Cluster::Cluster() : activeWorkers_(0) {}

Cluster::Cluster(Cluster const &) {}

Cluster &Cluster::operator=(Cluster const &) { return *this; }

Cluster::~Cluster() {
  for (std::list<ASocket*>::iterator i = sockets_.begin(); i != sockets_.end(); ++i) {
    delete *i;
  }
  for (std::list<IWork*>::iterator i = works_.begin(); i != works_.end(); ++i) {
    delete *i;
  }
  for (std::vector<Worker*>::iterator i = workers_.begin(); i != workers_.end(); ++i) {
    delete *i;
  }
  pthread_mutex_destroy(&socketMutex_);
  pthread_mutex_destroy(&selectMutex_);
  pthread_mutex_destroy(&worksMutex_);
  pthread_mutex_destroy(&activeWorkersMutex_);
}

void Cluster::incActiveWorkers() {
  ++activeWorkers_;
}

void Cluster::decActiveWorkers() {
  --activeWorkers_;
}
