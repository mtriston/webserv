//
// Created by mtriston on 01.05.2021.
//

#include "Request.hpp"

Request::Request() {}

Request::Request(const Request &x) : _headers(x._headers) {}

Request::~Request() {}

void Request::parseRequest(std::string request) {
  parseFirstLine(request);
  parseHeaders(request);
  _headers["body"] = request;
}

std::string const &Request::getMethod() { return _headers["method"]; }

std::string const &Request::getPath() { return _headers["path"]; }

std::string const &Request::getVersion() { return _headers["version"]; }

void Request::parseFirstLine(std::string &request) {
  std::string line = cut_next_token(request, "\r\n");
  _headers["method"] = cut_next_token(line, " ");
  _headers["path"] = cut_next_token(line, " ");
  _headers["version"] = cut_next_token(line, " ");
}

void Request::parseHeaders(std::string &request) {
  for (std::string line = cut_next_token(request, "\r\n"); !line.empty(); line = cut_next_token(request, "\r\n")) {
    std::string first = cut_next_token(line, ": ");
    _headers.insert(std::make_pair(ft_tolower(first), line));
  }
}
//TODO: Обработать POST запрос

//TODO: удалить этот метод
void Request::print() {
  std::cout << "REQUEST" << std::endl;
  std::map<std::string, std::string>::iterator begin = _headers.begin();
  std::map<std::string, std::string>::iterator end = _headers.end();
  while (begin != end) {
    std::cout << (*begin).first << ":: " << (*begin).second << std::endl;
    ++begin;
  }
  std::cout << std::endl << std::endl;
}

int Request::getContentLength() {
  return ft_atoi(_headers["data-length"]);
}
