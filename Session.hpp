//
// Created by mtriston on 30.04.2021.
//

#ifndef WEBSERV__SESSION_HPP_
#define WEBSERV__SESSION_HPP_
#define BUF_SIZE 2048

#include <iostream>
#include <fcntl.h> //неблокирующий режим
#include <unistd.h> // closeConnection()
#include <string>
#include <vector>
#include "Config.hpp"
#include "Request.hpp"
#include "Response.hpp"
#include "utils.hpp"

enum session_states {
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
  session_states getState() const;
  void readRequest();
  bool _isRequestRead();
  bool isReadyGenerateResponse(fd_set *readfds, fd_set *writefds) const;
  void generateResponse();
  void sendResponse();
  void closeConnection() const;
  int fillFdSet(fd_set *readfds, fd_set *writefds) const;
  
 private:
  Session();
  Session &operator=(Session const &);
  
  int _fd;
  enum session_states _state;
  std::string _buffer;
  Request _request;
  Response _response;
  const Config* _config;
};

#endif //WEBSERV__SESSION_HPP_
