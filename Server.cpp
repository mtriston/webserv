//
// Created by mtriston on 27.04.2021.
//

#include "Server.hpp"

Server::Server(char *ip, int port) : _ls(), _addr(), _sessions() {
  _addr.sin_family = AF_INET;
  _addr.sin_port = htons(port);
  _addr.sin_addr.s_addr = inet_addr(ip);
  if (_addr.sin_addr.s_addr == INADDR_NONE) {
    std::cerr << "inet_addr error" << std::endl; //TODO: решить, что делать при ошибке
  }
}

Server::Server(Server const &x) : _ls(x._ls), _addr(x._addr), _sessions(x._sessions) {}

Server::~Server() {}

int Server::get_socket() const { return _ls; }

int Server::set_fds(fd_set *readfds, fd_set *writefds) {
  int max_fd = _ls;
  FD_SET(_ls, readfds);
  std::map<int, Session>::iterator begin = _sessions.begin();
  std::map<int, Session>::iterator end = _sessions.end();
  while (begin != end) {
    Session &session = begin->second;
    if (session.get_state() == fsm_read) {
      FD_SET(session.get_socket(), readfds);
      max_fd = max_fd < session.get_socket() ? session.get_socket() : max_fd;
    } else if (session.get_state() == fsm_write) {
      FD_SET(session.get_socket(), writefds);
      max_fd = max_fd < session.get_socket() ? session.get_socket() : max_fd;
    }
    ++begin;
  }
  return max_fd;
}

void Server::run() {
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

  if (bind(_ls, (sockaddr *) &_addr, sizeof(_addr)) == -1) {
    std::cerr << "bind error" << std::endl; //TODO: решить, что делать при ошибке
  }
  if (listen(_ls, QLEN) == -1) {
    std::cerr << "listen error" << std::endl; //TODO: решить, что делать при ошибке
  }
}

int Server::accept() {
  int cls = ::accept(_ls, 0, 0);
  if (cls == -1) {
    std::cerr << "accept error" << std::endl; //TODO: решить, что делать при ошибке
  }
  _sessions.insert(std::make_pair(cls, Session(cls)));
  return cls;
}

void Server::finish() {
  std::map<int, Session>::iterator begin = _sessions.begin();
  std::map<int, Session>::iterator end = _sessions.end();
  while (begin != end) {
    begin->second.close();
  }
  _sessions.clear();
  close(_ls);
}

Server &Server::operator=(Server const &x) {
  (void) x;
  return *this;
}

Server::Server() {}

void Server::read_request(fd_set *readfds) {
  std::map<int, Session>::iterator begin = _sessions.begin();
  std::map<int, Session>::iterator end = _sessions.end();
  while (begin != end) {
    Session &session = begin->second;
    if (FD_ISSET(session.get_socket(), readfds)) {
      session.read_request();
    }
    ++begin;
  }
}

void Server::send_response(fd_set *writefds) {
  std::map<int, Session>::iterator begin = _sessions.begin();
  std::map<int, Session>::iterator end = _sessions.end();
  while (begin != end) {
    Session &session = begin->second;
    if (FD_ISSET(session.get_socket(), writefds)) {
      session.send_response();
      ++begin;
      int tmp = session.get_socket();
      session.close(); //TODO: не поддерживает соединение, сразу разрывает
      _sessions.erase(tmp);
    } else {
      ++begin;
    }
  }
}
