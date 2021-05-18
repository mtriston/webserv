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

class Response {
 public:
  Response();
  Response(Request *request, const Config *config);
  Response(Response const &);
  void generateResponse();
  std::string const &getResponse();
  ~Response();

 private:

  void _handleMethodGET();
  void _handleMethodHEAD();

  void _readContent();
  void _writeContent();
  void _analyzeContent();
  std::string _getContentType(std:: string const &);
  std::string _getContentLength(std::string const &);

  Request *_request;
  const Config *_config;
  struct content _content;
  std::string _response;
};

#endif //WEBSERV__RESPONSE_HPP_
