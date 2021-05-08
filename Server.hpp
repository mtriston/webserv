//
// Created by mtriston on 27.04.2021.
//

#ifndef WEBSERV__SERVER_HPP_
#define WEBSERV__SERVER_HPP_

#include "Session.hpp"
#include "Config.hpp"
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <list>

class Server {
 public:
  explicit Server(const Config *);
  Server(Server const &);
  ~Server();
  int getSocket() const;
  int setFds(fd_set *readfds, fd_set *writefds);
  void tryReadRequest(fd_set *readfds);
  void trySendResponse(fd_set *writefds);
  int acceptConnection();
  bool run();
  void finish();

 private:
  Server();
  Server &operator=(Server const &);

  int _ls;
  sockaddr_in _addr;
  const Config *_config;
  std::list<Session> _sessions;
};

#endif //WEBSERV__SERVER_HPP_
