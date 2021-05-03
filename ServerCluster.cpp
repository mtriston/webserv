//
// Created by mtriston on 26.04.2021.
//

#include "ServerCluster.hpp"

ServerCluster &ServerCluster::Instance() {
  static ServerCluster theSingleInstance;
  return theSingleInstance;
}

void ServerCluster::setup(std::vector<Config> const &configs) {
  std::vector<Config>::const_iterator b = configs.begin();
  std::vector<Config>::const_iterator e = configs.end();
  while (b != e) {
    Server server(*b);
    server.run();
    _servers.insert(std::make_pair(server.get_socket(), server));
    ++b;
  }
}

int ServerCluster::_set_fds(fd_set *readfds, fd_set* writefds) {
  int max_fd = _servers.begin()->first;
  FD_ZERO(readfds);
  FD_ZERO(writefds);
  std::map<int, Server>::iterator begin = _servers.begin();
  std::map<int, Server>::iterator end = _servers.end();
  while (begin != end) {
    Server &server = begin->second;
    max_fd = std::max(max_fd, server.set_fds(readfds, writefds));
    ++begin;
  }
  return max_fd;
}

void ServerCluster::_accept_connection(fd_set *readfds) {
  std::map<int, Server>::iterator begin = _servers.begin();
  std::map<int, Server>::iterator end = _servers.end();
  while (begin != end) {
    Server &server = begin->second;
    if (FD_ISSET(server.get_socket(), readfds)) {
      server.accept();
    }
    ++begin;
  }
}

void ServerCluster::_read_request(fd_set *readfds) {
  std::map<int, Server>::iterator begin = _servers.begin();
  std::map<int, Server>::iterator end = _servers.end();
  while (begin != end) {
    Server &server = begin->second;
    server.read_request(readfds);
    ++begin;
  }
}

void ServerCluster::_send_response(fd_set *writefds) {
  std::map<int, Server>::iterator begin = _servers.begin();
  std::map<int, Server>::iterator end = _servers.end();
  while (begin != end) {
    Server &server = begin->second;
    server.send_response(writefds);
    ++begin;
  }
}

void ServerCluster::run() {
  //TODO: Реализовать функционал.
  while (1) {
    fd_set readfds, writefds;
    int max_fd = _set_fds(&readfds, &writefds);
    int res = select(max_fd + 1, &readfds, &writefds, 0, 0);
    if (res == -1) {
      std::cerr << "Select error" << std::endl; //TODO: ???
      continue;
    } else if (res == 0) {
      continue; // тайм аут
    }
    this->_accept_connection(&readfds);
    this->_read_request(&readfds);
    this->_send_response(&writefds);
  }
}

void ServerCluster::finish() {
  std::map<int, Server>::iterator begin = _servers.begin();
  std::map<int, Server>::iterator end = _servers.end();
  while (begin != end) {
    begin->second.finish();
  }
}

ServerCluster::ServerCluster() {}

ServerCluster::ServerCluster(ServerCluster const &x) { (void) x; }

ServerCluster &ServerCluster::operator=(ServerCluster const &x) {
  (void) x;
  return *this;
}
