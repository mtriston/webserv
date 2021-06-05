//
// Created by mtriston on 01.05.2021.
//

#ifndef WEBSERV__REQUEST_HPP_
#define WEBSERV__REQUEST_HPP_

#include <string>
#include <map>
#include "utils.hpp"
#include <iostream> //TODO: delete this include and method print

class Request {
 public:
  Request();
  Request(Request const &);
  ~Request();
  void print();
  std::string const &getMethod();
  std::string const &getPath();
  std::string const &getVersion();
  int getContentLength();
  std::string getHost() const;
  void parseRequest(std::string );
 private:
  Request &operator=(Request const &);
  void parseFirstLine(std::string &);
  void parseHeaders(std::string &);
  std::map<std::string, std::string> _headers;
};

#endif //WEBSERV__REQUEST_HPP_
