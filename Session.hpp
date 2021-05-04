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
#include <vector>
#include "Config.hpp"
#include "Request.hpp"
#include "Response.hpp"
#include "utils.hpp"

enum fsm_states {
  fsm_read,
  fsm_write
};

class Session {
 public:
  Session(int, Config const &config);
  Session(Session const &);
  ~Session();

  int get_socket() const;
  fsm_states get_state() const;
  void send_response();
  void read_request();
  void close() const;
 private:
  Session();
  Session &operator=(Session const &);
  void generate_response();
  int _fd;
  char _buf[BUF_SIZE];
  enum fsm_states _state;
  Config config;
  std::string _received;
  std::string _response;
};

#endif //WEBSERV__SESSION_HPP_
