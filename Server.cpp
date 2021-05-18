//
// Created by mtriston on 27.04.2021.
//

#include "Server.hpp"

Server::Server(const Config *config) : _ls(), _config(config) {}

Server::Server(Server const &x) : _ls(x._ls), _config(x._config), _sessions(x._sessions) {}

Server::~Server() {}

int Server::getSocket() const { return _ls; }

int Server::setFds(fd_set *readfds, fd_set *writefds) {
  int max_fd = _ls;
  FD_SET(_ls, readfds);
  std::list<Session>::iterator b = _sessions.begin();
  std::list<Session>::iterator e = _sessions.end();
  while (b != e) {
    if (b->getState() == READ_REQUEST) {
      FD_SET(b->getSocket(), readfds);
      max_fd = std::max(b->getSocket(), max_fd);
    } else if (b->getState() == SEND_RESPONSE) {
      FD_SET(b->getSocket(), writefds);
      max_fd = std::max(b->getSocket(), max_fd);
    }
    ++b;
  }
  return max_fd;
}

bool Server::run() {

  std::clog << "Trying to start a server on " + _config->getIP() + ":" << _config->getPort() << std::endl;

  _ls = socket(AF_INET, SOCK_STREAM, 0);
  if (_ls == -1) {
    std::cerr << "Error creating socket" << std::endl;
    return false;
  }

  //Для избежания залипания порта.
  int opt = 1;
  setsockopt(_ls, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

  struct sockaddr_in addr = {};
  addr.sin_family = AF_INET;
  addr.sin_port = htons(_config->getPort());
  addr.sin_addr.s_addr = inet_addr(_config->getIP().c_str());

  if (bind(_ls, (sockaddr *) &addr, sizeof(addr)) == -1) {
    std::clog << "Error binding socket" << std::endl;
    return false;
  }
  if (listen(_ls, _config->getQueueLength()) == -1) {
    std::clog << "Error listening socket" << std::endl;
    return false;
  }
  std::clog << "Server started" << std::endl << std::endl;
  return true;
}

int Server::acceptConnection() {

  int cls = ::accept(_ls, 0, 0);
  if (cls == -1) {
    std::cerr << "Error accepting connection to " << _config->getIP() << ":" << _config->getPort() << std::endl;
  } else {
    _sessions.push_back(Session(cls, _config));
  }
  return cls;
}

void Server::finish() {
  std::list<Session>::iterator b = _sessions.begin();
  std::list<Session>::iterator e = _sessions.end();
  while (b != e) {
    b->closeConnection();
    ++b;
  }
  _sessions.clear();
  close(_ls);
}

void Server::tryReadRequest(fd_set *readfds) {
  std::list<Session>::iterator b = _sessions.begin();
  std::list<Session>::iterator e = _sessions.end();
  while (b != e) {
    if (FD_ISSET(b->getSocket(), readfds)) {
      FD_CLR(b->getSocket(), readfds);
      b->readRequest();
    }
    ++b;
  }
}

void Server::trySendResponse(fd_set *writefds) {
  std::list<Session>::iterator b = _sessions.begin();
  std::list<Session>::iterator e = _sessions.end();
  while (b != e) {
    if (FD_ISSET(b->getSocket(), writefds)) {
      FD_CLR(b->getSocket(), writefds);
      b->sendResponse();
      if (b->getState() == CLOSE_CONNECTION) {
        b->closeConnection();
        b = _sessions.erase(b);
        continue;
      }
    }
    ++b;
  }
}

Server &Server::operator=(Server const &) { return *this; }

Server::Server() {}