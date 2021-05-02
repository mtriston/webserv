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
  Request(std::string const &);
  Request(Request const &);
  ~Request();
  void print();
 private:
  Request();
  Request &operator=(Request const &);
  void parse_first_line();
  void parse_headers();
  std::string request;
  std::string method;
  std::string path;
  std::string version;
  std::map<std::string, std::string> headers;
};

#endif //WEBSERV__REQUEST_HPP_
