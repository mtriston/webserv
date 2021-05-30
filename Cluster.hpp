#include <cassert>
//
// Created by mtriston on 26.04.2021.
//

#ifndef WEBSERV__CLUSTER_HPP_
#define WEBSERV__CLUSTER_HPP_

class Config;

#include "SocketList.hpp"
#include "WorkList.hpp"
#include "WorkerManager.hpp"

#include <vector>
#include <list>
#include <algorithm>
#include <pthread.h>
#include <ctime> //TODO: ???
#include <sys/types.h>
#include <unistd.h>
#include <iostream>

class Cluster {
public:
    Cluster();

    ~Cluster();

    void setup(std::vector<Config> &configs);

    void run();

private:
    SocketList *socketList_;
    WorkList *workList_;
    WorkerManager *workerManager_;

    Cluster(Cluster const &);

    Cluster &operator=(Cluster const &);
};

#endif //WEBSERV__CLUSTER_HPP_
