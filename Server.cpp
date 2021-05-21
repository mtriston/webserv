//
// Created by mtriston on 27.04.2021.
//

#include "Server.hpp"

Server::Server(Config *config) : ASocket(0, config) {}

Server::~Server() {
  delete config_;
  close(socket_);
}

int Server::fillFdSet(fd_set *readfds, fd_set *) {
  FD_SET(socket_, readfds);
  return socket_;
}

bool Server::run() {

  std::clog << "Trying to start a server on " + config_->getIP() + ":" << config_->getPort() << std::endl;

  socket_ = socket(AF_INET, SOCK_STREAM, 0);
  if (socket_ == -1) {
    std::cerr << "Error creating socket" << std::endl;
    return false;
  }

  //Для избежания залипания порта.
  int opt = 1;
  setsockopt(socket_, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

  struct sockaddr_in addr = {};
  addr.sin_family = AF_INET;
  addr.sin_port = htons(config_->getPort());
  addr.sin_addr.s_addr = inet_addr(config_->getIP().c_str());

  if (bind(socket_, (sockaddr *) &addr, sizeof(addr)) == -1) {
    std::clog << "Error binding socket" << std::endl;
    return false;
  }
  if (listen(socket_, config_->getQueueLength()) == -1) {
    std::clog << "Error listening socket" << std::endl;
    return false;
  }
  std::clog << "Server started" << std::endl << std::endl;
  return true;
}

int Server::acceptConnection() {

  int cls = ::accept(socket_, 0, 0);
  if (cls == -1) {
    std::cerr << "Error accepting connection" << std::endl;
  } else {
    fcntl(cls, F_SETFL, O_NONBLOCK);
  }
  return cls;
}

	bool Server::isReady(fd_set *readfds, fd_set *) {
    return FD_ISSET(socket_, readfds);
  }

	IWork *Server::makeWork() {
    return new AcceptConntectionWork(this);
  }

Server::Server(Server const &) {}

Server &Server::operator=(Server const &) { return *this; }

Server::Server() {}
