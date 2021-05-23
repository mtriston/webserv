//
// Created by mtriston on 27.04.2021.
//


#include "ListenSocket.hpp"
#include "Config.hpp"
#include "IWork.hpp"

ListenSocket::ListenSocket(Config *config) : ASocket(0, config) {}

ListenSocket::~ListenSocket() {
  delete config_;
  close(socket_);
}

int ListenSocket::fillFdSet(fd_set *readfds, fd_set *) {
  FD_SET(socket_, readfds);
  return socket_;
}

bool ListenSocket::run() {

  std::clog << "Trying to run a server on " + config_->getIP() + ":" << config_->getPort() << std::endl;

  socket_ = socket(AF_INET, SOCK_STREAM, 0);
  if (socket_ == -1) {
    std::cerr << "Error creating socket" << std::endl;
    return false;
  }

  fcntl(socket_, F_SETFL, O_NONBLOCK);

  //Для избежания залипания порта.
  int opt = 1;
  setsockopt(socket_, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

  struct sockaddr_in addr = {};
  addr.sin_family = AF_INET;
  addr.sin_port = htons(config_->getPort());
  addr.sin_addr.s_addr = inet_addr(config_->getIP().c_str());

  if (bind(socket_, reinterpret_cast<sockaddr *>(&addr), sizeof(addr)) == -1) {
    std::clog << "Error binding socket" << std::endl;
    return false;
  }
  if (listen(socket_, config_->getQueueLength()) == -1) {
    std::clog << "Error listening socket" << std::endl;
    return false;
  }
  std::clog << "ListenSocket started" << std::endl << std::endl;
  return true;
}

int ListenSocket::acceptConnection() {

  int cls = accept(socket_, 0, 0);
  if (cls != -1) {
    fcntl(cls, F_SETFL, O_NONBLOCK);
  }
  return cls;
}

bool ListenSocket::isReady(fd_set *readfds, fd_set *) {
  return !isBusy_ && FD_ISSET(socket_, readfds);
}

IWork *ListenSocket::getWork() {
  isBusy_ = true;
  return new AcceptConnectionWork(this);
}

ListenSocket::ListenSocket(ListenSocket const &) {}

ListenSocket &ListenSocket::operator=(ListenSocket const &) { return *this; }

ListenSocket::ListenSocket() {}
