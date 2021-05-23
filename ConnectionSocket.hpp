//
// Created by mtriston on 30.04.2021.
//

#ifndef WEBSERV__CONNECTIONSOCKET_HPP_
#define WEBSERV__CONNECTIONSOCKET_HPP_


#define BUF_SIZE 2048
#include <string>
#include <sys/types.h>
#include <unistd.h>
#include <iostream>
#include "ASocket.hpp"
#include "Response.hpp"
#include "Request.hpp"

enum session_states {
  READ_REQUEST,
  GENERATE_RESPONSE,
  SEND_RESPONSE,
  CLOSE_CONNECTION
};

class ConnectionSocket : public ASocket {
 public:
  ConnectionSocket(int, Config *);
  ~ConnectionSocket();

  session_states getState() const;
  void readRequest();
  void generateResponse();
  void sendResponse();
  bool _isRequestRead();
  int fillFdSet(fd_set *readfds, fd_set *writefds);
	bool isReady(fd_set *readfds, fd_set *writefds);
	IWork *getWork();
  
 private:
  ConnectionSocket();
  ConnectionSocket(ConnectionSocket const &);
  ConnectionSocket &operator=(ConnectionSocket const &);

  enum session_states _state;
  std::string _buffer;
  Request _request;
  Response _response;
};

#endif //WEBSERV__CONNECTIONSOCKET_HPP_
