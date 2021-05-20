//
// Created by mtriston on 26.04.2021.
//

#include "ServerCluster.hpp"

void ServerCluster::setup(std::vector<Config> const &configs) {
  std::vector<Config>::const_iterator b = configs.begin();
  std::vector<Config>::const_iterator e = configs.end();
  while (b != e) {
      Server server(&(*b));
      if (server.run()) {
        _servers.push_back(server);
      }
    ++b;
  }
}

void ServerCluster::_buildToDoList() {
  for (std::list<ASocket*>::iterator i = sockets_.begin(); i != sockets_.end(); ++i) {
    if (FD_ISSET((*i)->getSocket(), readfds) || FD_ISSET((*i)->getSocket(), writefds)) {
      tasks_.push_back((*i)->makeStrategy());
    }
  }  
}

void ServerCluster::run() {
  while (1) {
    fd_set readfds, writefds;
    int max_fd = _fillFdSet(&readfds, &writefds);
    int res = select(max_fd + 1, &readfds, &writefds, 0, 0);
    if (res == -1) {
      std::cerr << "Select error" << std::endl; //TODO: ???
      continue;
    } else if (res == 0) {
      continue; // тайм аут
    }
    _buildToDoList();
    
  }
}

void ServerCluster::finish() {
  std::vector<Server>::iterator b = _servers.begin();
  std::vector<Server>::iterator e = _servers.end();
  while (b != e) {
    b->finish();
    ++b;
  }
}

int ServerCluster::_fillFdSet(fd_set *readfds, fd_set* writefds) {
  int max_fd = _servers.front().getSocket();
  FD_ZERO(readfds);
  FD_ZERO(writefds);
  std::vector<Server>::iterator b = _servers.begin();
  std::vector<Server>::iterator e = _servers.end();
  while (b != e) {
    max_fd = std::max(max_fd, b->fillFdSet(readfds, writefds));
    ++b;
  }
  return max_fd;
}

void ServerCluster::_tryAcceptConnection(fd_set *readfds) {
  std::vector<Server>::iterator b = _servers.begin();
  std::vector<Server>::iterator e = _servers.end();
  while (b != e) {
    if (FD_ISSET(b->getSocket(), readfds)) {
      b->acceptConnection();
    }
    ++b;
  }
}

void ServerCluster::_tryReadRequest(fd_set *readfds) {
  std::vector<Server>::iterator b = _servers.begin();
  std::vector<Server>::iterator e = _servers.end();
  while (b != e) {
    b->tryReadRequest(readfds);
    ++b;
  }
}

void ServerCluster::_tryGenerateResponse(fd_set *readfds, fd_set *writefds) {
  std::vector<Server>::iterator b = _servers.begin();
  std::vector<Server>::iterator e = _servers.end();
  while (b != e) {
    b->tryGenerateResponse(readfds, writefds);
    ++b;
  }
}

void ServerCluster::_trySendResponse(fd_set *writefds) {
  std::vector<Server>::iterator b = _servers.begin();
  std::vector<Server>::iterator e = _servers.end();
  while (b != e) {
    b->trySendResponse(writefds);
    ++b;
  }
}

ServerCluster::ServerCluster() {}

ServerCluster::ServerCluster(ServerCluster const &) {}

ServerCluster &ServerCluster::operator=(ServerCluster const &) { return *this; }

ServerCluster::~ServerCluster() {}
