//
// Created by mtriston on 26.04.2021.
//

#ifndef WEBSERV__CLUSTER_HPP_
#define WEBSERV__CLUSTER_HPP_

class IWork;
class Worker;
class Config;
class ASocket;

#include <vector>
#include <list>
#include <algorithm>
#include <pthread.h>
#include <ctime> //TODO: ???
#include <sys/types.h>
#include <unistd.h>
#include <iostream>

#define COUNT_OF_WORKERS 4 //TODO: заменить на данные из конфиг файла

class Cluster {
 public:
  Cluster();
  ~Cluster();
  void setup(std::vector<Config> &configs);
  void run();
  void addSocket(ASocket *);
  void removeSocket(ASocket *);
  void incActiveWorkers();
  void decActiveWorkers();
  IWork *getWork();

 private:
  int activeWorkers_;
  std::list<ASocket*> sockets_;
  std::list<IWork*> works_;
  std::vector<Worker*> workers_;
  pthread_mutex_t socketMutex_;
  pthread_mutex_t worksMutex_;
  pthread_mutex_t selectMutex_;
  pthread_mutex_t activeWorkersMutex_;

  Cluster(Cluster const &);
  Cluster &operator=(Cluster const &);
};

#endif //WEBSERV__CLUSTER_HPP_
