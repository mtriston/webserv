//
// Created by mtriston on 01.05.2021.
//

#include "Request.hpp"

Request::Request() {}

Request::Request(std::string const &request) : request(request) {
  parse_first_line();
  parse_headers();
}

Request::Request(const Request &x) : request(x.request), method(x.method), path(x.path), version(x.version), headers(x.headers) {}

Request::~Request() {}

void Request::parse_first_line() {
  std::string line = cut_next_token(request, "\r\n");
  method = cut_next_token(line, " ");
  path = cut_next_token(line, " ");
  version = cut_next_token(line, " ");
}

void Request::parse_headers() {
  for (std::string line = cut_next_token(request, "\r\n"); !line.empty(); line = cut_next_token(request, "\r\n")) {
    std::string first = cut_next_token(line, ": ");
    headers.insert(std::make_pair(first, line));
  }
}

//TODO: удалить этот метод
void Request::print() {
  std::cout << "REQUEST" << std::endl;
  std::cout << "method = " << method << std::endl;
  std::cout << "path = " << path << std::endl;
  std::cout << "version = " << version << std::endl;
  std::cout << "HEADERS" << std::endl;
  std::map<std::string, std::string>::iterator begin = headers.begin();
  std::map<std::string, std::string>::iterator end = headers.end();
  while (begin != end) {
    std::cout << (*begin).first << ": " << (*begin).second << std::endl;
    ++begin;
  }
  std::cout << std::endl << std::endl;
}
