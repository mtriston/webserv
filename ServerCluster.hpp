//
// Created by mtriston on 26.04.2021.
//

#ifndef WEBSERV__SERVERCLUSTER_HPP_
#define WEBSERV__SERVERCLUSTER_HPP_

#include "Server.hpp"
#include "ConfigParser.hpp"
#include <iostream>
#include <unistd.h>
#include <map>

//TODO: заменить define на данные из когфиг файла

class ServerCluster {
 public:
  static ServerCluster& Instance();
  void setup(std::vector<Config> const &configs);
  void run();
  void finish();

 private:
  std::map<int, Server> _servers;

  int _set_fds(fd_set *readfds, fd_set *writefds);
  ServerCluster();
  ServerCluster(ServerCluster const &);
  ServerCluster& operator=(ServerCluster const &);
  void _accept_connection(fd_set *readfds);
  void _read_request(fd_set *readfds);
  void _send_response(fd_set *writefds);
};

#endif //WEBSERV__SERVERCLUSTER_HPP_
