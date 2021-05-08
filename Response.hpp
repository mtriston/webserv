//
// Created by mtriston on 04.05.2021.
//

#ifndef WEBSERV__RESPONSE_HPP_
#define WEBSERV__RESPONSE_HPP_

#include "Config.hpp"
#include "Request.hpp"
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

class Response {
 public:
  Response(Request *request, const Config *config);
  Response(Response const &);
  std::basic_string<char> const &getResponse();
  ~Response();

 private:
  std::string _getStatusLine();
  std::string _getStatusCode();
  std::basic_string<char> _getContent(std::string const &);
  std::string _getContentType(std:: string const &);
  std::string _getContentLength(std::string const &);

  Request *request_;
  const Config *config_;
  std::basic_string<char> response_;

  Response();
};

#endif //WEBSERV__RESPONSE_HPP_
