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
#include "ASocket.hpp"
#include "Request.hpp"
#include "Response.hpp"
#include "utils.hpp"

enum session_states {
  READ_REQUEST,
  GENERATE_RESPONSE,
  SEND_RESPONSE,
  CLOSE_CONNECTION
};

class Session : public ASocket {
 public:
  Session(int, Config *);
  ~Session();

  session_states getState() const;
  void readRequest();
  void generateResponse();
  void sendResponse();
  bool _isRequestRead();
  bool isReadyGenerateResponse(fd_set *readfds, fd_set *writefds) const;
  int fillFdSet(fd_set *readfds, fd_set *writefds);
	bool isReady(fd_set *readfds, fd_set *writefds);
	IWork *makeWork();
  
 private:
  Session();
  Session(Session const &);
  Session &operator=(Session const &);

  enum session_states _state;
  std::string _buffer;
  Request _request;
  Response _response;
};

#endif //WEBSERV__SESSION_HPP_
