//
// Created by mtriston on 26.04.2021.
//

#ifndef WEBSERV__SERVERCLUSTER_HPP_
#define WEBSERV__SERVERCLUSTER_HPP_

#include "Server.hpp"
#include "ASocket.hpp"
#include <iostream>
#include <unistd.h>
#include <vector>

class ServerCluster {
 public:
  ServerCluster();
  ~ServerCluster();
  void setup(std::vector<Config> const &configs);
  void run();
  void finish();

 private:
//  std::vector<Server> _servers;
  std::list<ASocket*> sockets_;
  std::list<IStrategy*> tasks_;

  int _fillFdSet(fd_set *readfds, fd_set *writefds);
  void _buildToDoList();
  // void _tryAcceptConnection(fd_set *readfds);
  // void _tryReadRequest(fd_set *readfds);
  // void _tryGenerateResponse(fd_set *readfds, fd_set *writefds);
  // void _trySendResponse(fd_set *writefds);

  ServerCluster(ServerCluster const &);
  ServerCluster &operator=(ServerCluster const &);
};

#endif //WEBSERV__SERVERCLUSTER_HPP_
