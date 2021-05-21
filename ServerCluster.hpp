//
// Created by mtriston on 26.04.2021.
//

#ifndef WEBSERV__SERVERCLUSTER_HPP_
#define WEBSERV__SERVERCLUSTER_HPP_

#include "Server.hpp"
#include "Session.hpp"
#include "Worker.hpp"
#include <vector>
#include <list>
#include <pthread.h>

class IWork;
class Worker;

class ServerCluster {
 public:
  ServerCluster();
  ~ServerCluster();
  void setup(std::vector<Config> &configs);
  void run();
  void finish();
  void unlockSelect();
  void addSocket(ASocket *);
  void removeSocket(ASocket *);
  IWork *getWork();

 private:
  std::list<ASocket*> sockets_;
  std::list<IWork*> works_;
  std::vector<Worker*> workers_;
  pthread_mutex_t socketLock_;
  pthread_mutex_t worksLock_;
  pthread_mutex_t selectLock_;
  
  ServerCluster(ServerCluster const &);
  ServerCluster &operator=(ServerCluster const &);
};

#endif //WEBSERV__SERVERCLUSTER_HPP_
