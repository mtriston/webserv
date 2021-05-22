//
// Created by mtriston on 04.05.2021.
//

#ifndef WEBSERV__RESPONSE_HPP_
#define WEBSERV__RESPONSE_HPP_

#include <string>
#include <sys/types.h>
#include <sys/stat.h>
#include <cerrno>
#include <fcntl.h>
#include <unistd.h>
#include <iostream>
#include <cstdlib> //TODO: удалить и заменить atoi
#include <sstream> //TODO: удалить и заменить
#include "utils.hpp"
#include "Request.hpp"
#include "Config.hpp"

struct content {
 public:
  std::string file;
  std::string data;
  std::string contentLength;
  std::string contentType;
  std::string lastModified;
  std::string status;
};

enum response_states {
  GENERATE_HEADERS,
  READ_FILE,
  READ_CGI,
  WRITE_FILE,
  READY_FOR_SEND
};

class Response {
 public:
  Response();
  Response(Response const &);
  int fillFdSet(fd_set *readfds, fd_set *writefds) const;
  void initGenerateResponse(Request *request, const Config *config);
  void generateResponse();
  std::string const &getResponse() const;
  bool isReadyGenerate(fd_set *readfds, fd_set *writefds) const;
  bool isGenerated() const;
  ~Response();

 private:

  void _handleMethodGET();
  void _handleMethodHEAD();

  void _readContent();
  void _writeContent();
  std::string _getContentType(std:: string const &);

  Request *_request;
  const Config *_config;
  struct content _content;
  std::string _response;
  int fd;
  response_states _state;
};

#endif //WEBSERV__RESPONSE_HPP_
