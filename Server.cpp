//
// Created by mtriston on 27.04.2021.
//

#include "Server.hpp"

Server::Server(char *ip, int port) {
  _ls = socket(AF_INET, SOCK_STREAM, 0);
  if (_ls == -1) {
    std::cerr << "socket error" << std::endl; //TODO: решить, что делать при ошибке
  }
  /*
 * Для избежания залипания порта.
 * TODO: Решить, стоит ли оставлять в готовой программе
 */
  int opt = 1;
  setsockopt(_ls, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
  struct sockaddr_in addr;
  addr.sin_family = AF_INET;
  addr.sin_port = htons(port);
  addr.sin_addr.s_addr = inet_addr(ip);
  if (addr.sin_addr.s_addr == INADDR_NONE) {
    std::cerr << "inet_addr error" << std::endl; //TODO: решить, что делать при ошибке
  }
  if (bind(_ls, ( sockaddr*)&addr, sizeof(addr)) == -1) {
    std::cerr << "bind error" << std::endl; //TODO: решить, что делать при ошибке
  }
  if (listen(_ls, QLEN) == -1) {
    std::cerr << "listen error" << std::endl; //TODO: решить, что делать при ошибке
  }
}

Server::Server(Server const &x) {
  _ls = x._ls;
}

Server::~Server() {
  //TODO: Возможно, нужно сохранять все открытые соединения и закрывать их тут
}

int Server::getLS() const { return _ls; }

int Server::accept() {

  int cls = ::accept(_ls,0, 0);
  if (cls == -1) {
    std::cerr << "accept error" << std::endl; //TODO: решить, что делать при ошибке
  }
  sessions.insert(std::make_pair(cls, Session(cls)));
  return cls;
}

void Server::closeSession(int fd) {
  close(fd);
  sessions.erase(fd);
}

void Server::finish() {
  std::map<int, Session>::iterator begin = sessions.begin();
  std::map<int, Session>::iterator end = sessions.end();
  while (begin != end) {
    close(begin->first);
  }
  sessions.clear();
  close(_ls);
}

Server &Server::operator=(Server const &x) { (void)x; return *this; }
Server::Server() {}
