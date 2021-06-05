#include <cassert>
//
// Created by mtriston on 26.04.2021.
//

#ifndef WEBSERV__CLUSTER_HPP_
#define WEBSERV__CLUSTER_HPP_

#define DEFAULT_CONFIG_PATH "webserv.conf"

class SocketList;

class WorkList;

class WorkerManager;

class Config_parser;

#include <ctime>
#include <sys/types.h>
#include <iostream>

class Cluster {
public:
	Cluster();

	~Cluster();

	bool setup(std::string const &configFile);

	void run();

private:
	SocketList *socketList_;
	WorkList *workList_;
	WorkerManager *workerManager_;
	Config_parser *configParser_;

	Cluster(Cluster const &);

	Cluster &operator=(Cluster const &);
};

#endif //WEBSERV__CLUSTER_HPP_
