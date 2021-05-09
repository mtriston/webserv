//
// Created by mtriston on 03.05.2021.
//

#ifndef WEBSERV__CONFIG_HPP_
#define WEBSERV__CONFIG_HPP_

#include <iostream>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <vector>
#include <string>
#include <unistd.h>

class Config {
 public:
  Config();
  Config(std::string const &ip, int port);
  Config(Config const &);
  Config &operator=(Config const &);
  ~Config();
  std::string const &getIP() const;
  int const &getPort() const;
  int const &limitClientBodySize() const;
  std::vector<std::string> const &getIndex() const;
  std::string const &getRoot() const;
  int getQueueLength() const;
 private:
  std::string ip;
  int port;
  int limit_client_body_size;
  std::vector<std::string> index;
  std::string root;
  static const int queue_len = 16;
};

#endif //WEBSERV__CONFIG_HPP_
