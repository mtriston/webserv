//
// Created by mtriston on 04.05.2021.
//

#ifndef WEBSERV__RESPONSE_HPP_
#define WEBSERV__RESPONSE_HPP_

#include "Config.hpp"
#include "Request.hpp"
#include <fstream>
#include <sstream>
#include <sys/stat.h>
#include <cstring>

class Response {
 public:
  Response(Request *request, const Config *config);
  Response(Response const &);
  void generateResponse();
  std::string const &getResponse();
  ~Response();

 private:

  void _handleMethodGET();

  void _readContent();
  std::string _getContentType(std:: string const &);
  std::string _getContentLength(std::string const &);

  Request *_request;
  const Config *_config;
  std::string _response;
  int _status;
  std::string _mimeType;


  Response();
};

#endif //WEBSERV__RESPONSE_HPP_
