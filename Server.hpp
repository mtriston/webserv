//
// Created by mtriston on 27.04.2021.
//

#ifndef WEBSERV__SERVER_HPP_
#define WEBSERV__SERVER_HPP_

class Config;
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <iostream>
#include "ASocket.hpp"

class Server : public ASocket {
 public:
  Server(Config *);
  ~Server();
  bool run();
  int acceptConnection();
  int fillFdSet(fd_set *readfds, fd_set *writefds);
  bool isReady(fd_set *readfds, fd_set *writefds);
  IWork *makeWork();

 private:
  Server();
  Server(Server const &);
  Server &operator=(Server const &);
};

#endif //WEBSERV__SERVER_HPP_
