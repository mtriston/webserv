//
// Created by mtriston on 27.04.2021.
//

#ifndef WEBSERV__LISTENSOCKET_HPP_
#define WEBSERV__LISTENSOCKET_HPP_

class Config;
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <iostream>
#include "ASocket.hpp"

class ListenSocket : public ASocket {
 public:
  explicit ListenSocket(Config *);
  ~ListenSocket();
  bool run();
  int acceptConnection();
  int fillFdSet(fd_set *readfds, fd_set *writefds);
  bool isReady(fd_set *readfds, fd_set *writefds);
  IWork *getWork();

 private:
  ListenSocket();
  ListenSocket(ListenSocket const &);
  ListenSocket &operator=(ListenSocket const &);
};

#endif //WEBSERV__LISTENSOCKET_HPP_
