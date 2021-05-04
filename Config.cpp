//
// Created by mtriston on 03.05.2021.
//

#include "Config.hpp"

Config::Config() : ip("127.0.0.1"), port(4444), limit_client_body_size(1000000), root("/www") {
  if (inet_addr(ip.c_str()) == INADDR_NONE) {
    std::cerr << "inet_addr error" << std::endl; //TODO: решить, что делать при ошибке
  }
  index.push_back("index.html");
}

Config::Config(const std::string &ip, int port) : ip(ip), port(port), limit_client_body_size(1000000), root("/www") {
  if (inet_addr(ip.c_str()) == INADDR_NONE) {
    std::cerr << "inet_addr error" << std::endl; //TODO: решить, что делать при ошибке
  }
  index.push_back("index.html");
}

Config::~Config() {}

Config::Config(const Config &x) : ip(x.ip), port(x.port), limit_client_body_size(x.limit_client_body_size), index(x.index), root(x.root) {}

Config &Config::operator=(const Config &x) {
  if (this != &x) {
    ip = x.ip;
    port = x.port;
    limit_client_body_size = x.limit_client_body_size;
    index = x.index;
	root = x.root;
  }
  return *this;
}

std::string const &Config::getIP() const { return ip; }

int const &Config::getPort() const { return port; }

int const &Config::limitClientBodySize() const { return limit_client_body_size; }

std::vector<std::string> const &Config::getIndex() const { return index; }

std::string const &Config::getRoot() const { return root; }

int Config::getQueueLength() const { return queue_len; }
