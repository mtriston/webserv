//
// Created by mtriston on 26.04.2021.
//

#ifndef WEBSERV__SERVERCLUSTER_HPP_
#define WEBSERV__SERVERCLUSTER_HPP_

#include "Server.hpp"
#include <iostream>
#include <unistd.h>
#include <list>

class ServerCluster {
 public:
  ServerCluster();
  ~ServerCluster();
  void setup(std::vector<Config> const &configs);
  void run();
  void finish();

 private:
  std::list<Server> _servers;

  int _setFds(fd_set *readfds, fd_set *writefds);
  void _tryAcceptConnection(fd_set *readfds);
  void _tryReadRequest(fd_set *readfds);
  void _trySendResponse(fd_set *writefds);

  ServerCluster(ServerCluster const &);
  ServerCluster &operator=(ServerCluster const &);
};

#endif //WEBSERV__SERVERCLUSTER_HPP_
