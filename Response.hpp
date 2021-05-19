//
// Created by mtriston on 04.05.2021.
//

#ifndef WEBSERV__RESPONSE_HPP_
#define WEBSERV__RESPONSE_HPP_

#include "Config.hpp"
#include "Request.hpp"
#include <fcntl.h>
#include <sstream>
#include <sys/stat.h>
#include <cstring>
#include "utils.hpp"

struct content {
 public:
  std::string file;
  std::string data;
  std::string contentLength;
  std::string contentType;
  std::string lastModified;
  std::string status;
};

enum states {
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
  int getFd() const;
  void initGenerateResponse(Request *request, const Config *config);
  void generateResponse();
  std::string const &getResponse() const;
  bool isGenerated() const;
  bool isNeedToRead() const;
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
  states _state;
};

#endif //WEBSERV__RESPONSE_HPP_
