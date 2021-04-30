//
// Created by mtriston on 26.04.2021.
//

#ifndef WEBSERV__SERVERCLUSTER_HPP_
#define WEBSERV__SERVERCLUSTER_HPP_

#include "Server.hpp"

#include <iostream>
#include <unistd.h>
#include <map>

//TODO: заменить define на данные из когфиг файла
#define PORT 4444
#define IP "192.168.1.68"
#define IP2 "127.0.0.1"

class ServerCluster {
 public:
  static ServerCluster& Instance();
  void setup();
  void run();
  void finish();

 private:
  std::map<int, Server> _servers;

  ServerCluster();
  ServerCluster(ServerCluster const &);
  ServerCluster& operator=(ServerCluster const &);
};

#endif //WEBSERV__SERVERCLUSTER_HPP_
