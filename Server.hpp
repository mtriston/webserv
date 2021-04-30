//
// Created by mtriston on 27.04.2021.
//

#ifndef WEBSERV__SERVER_HPP_
#define WEBSERV__SERVER_HPP_

#include "Session.hpp"
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h> // close
#include <map>

#define QLEN 16 //TODO: Максимальная очередь, заменить дефайн на конфиг

class Server {
 public:
  Server(char *ip, int port);
  Server(Server const &);
  ~Server();
  int getLS() const;
  int accept();
  void closeSession(int);
  void finish();
  std::map<int, Session> sessions;

 private:
  Server();
  Server &operator=(Server const &x);
  int _ls;

};

#endif //WEBSERV__SERVER_HPP_
