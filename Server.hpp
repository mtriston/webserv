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
#include <unistd.h> // close
#include <map>

class Server {
 private:
  Server();
  Server &operator=(Server const &x);
 public:
  explicit Server(Config const &config);
  Server(Server const &);
  ~Server();
  int get_socket() const;
  int set_fds(fd_set *readfds, fd_set *writefds);
  void read_request(fd_set *readfds);
  void send_response(fd_set *writefds);
  void run();
  int accept();
  void finish();

 private:
  int _ls;
  sockaddr_in _addr;
  Config _config;
  std::map<int, Session> _sessions;
};

#endif //WEBSERV__SERVER_HPP_
