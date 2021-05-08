//
// Created by mtriston on 30.04.2021.
//

#ifndef WEBSERV__SESSION_HPP_
#define WEBSERV__SESSION_HPP_
#define BUF_SIZE 1460

#include <iostream>
#include <fcntl.h> //неблокирующий режим
#include <unistd.h> // closeConnection()
#include <cstring> // memset, strncpy
#include <string>
#include <vector>
#include "Config.hpp"
#include "Request.hpp"
#include "Response.hpp"
#include "utils.hpp"

enum fsm_states {
  fsm_read,
  fsm_write,
  fsm_close
};

class Session {
 public:
  Session(int, const Config *);
  Session(Session const &);
  ~Session();

  int getSocket() const;
  fsm_states getState() const;
  void sendResponse();
  void readRequest();
  void closeConnection() const;
  
 private:
  Session();
  Session &operator=(Session const &);
  void _generateResponse();
  
  int _fd;
  enum fsm_states _state;
  const Config* _config;
  std::string _request;
  std::basic_string<char> _response;
};

#endif //WEBSERV__SESSION_HPP_
