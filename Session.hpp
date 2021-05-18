//
// Created by mtriston on 30.04.2021.
//

#ifndef WEBSERV__SESSION_HPP_
#define WEBSERV__SESSION_HPP_
#define BUF_SIZE 1460

#include <iostream>
#include <fcntl.h> //неблокирующий режим
#include <unistd.h> // closeConnection()
#include <string>
#include <vector>
#include "Config.hpp"
#include "Request.hpp"
#include "Response.hpp"
#include "utils.hpp"

enum fsm_states {
  READ_REQUEST,
  GENERATE_RESPONSE,
  SEND_RESPONSE,
  CLOSE_CONNECTION
};

class Session {
 public:
  Session(int, const Config *);
  Session(Session const &);
  ~Session();

  int getSocket() const;
  fsm_states getState() const;
  void readRequest();
  void generateResponse();
  void sendResponse();
  void closeConnection() const;
  
 private:
  Session();
  Session &operator=(Session const &);
  
  int _fd;
  enum fsm_states _state;
  std::string _buffer;
  Request _request;
  Response _response;
  const Config* _config;
};

#endif //WEBSERV__SESSION_HPP_
