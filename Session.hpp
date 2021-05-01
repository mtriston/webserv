//
// Created by mtriston on 30.04.2021.
//

#ifndef WEBSERV__SESSION_HPP_
#define WEBSERV__SESSION_HPP_
#define BUF_SIZE 1460
#include <iostream>
#include <fcntl.h> //неблокирующий режим
#include <unistd.h> // close()
#include <cstring> // memset, strncpy
#include <string>

enum fsm_states {
  fsm_read,
  fsm_write
};

class Session {
 private:
  Session();
  Session &operator=(Session const &);
 public:
  Session(int);
  Session(Session const &);
  ~Session();

  int get_socket() const;
  fsm_states get_state() const;
  void send_response();
  void read_request();
  void close() const;

 private:
  int _fd;
  char _buf[BUF_SIZE];
  enum fsm_states _state;
  std::string _request;
  std::string _response;
};

#endif //WEBSERV__SESSION_HPP_
