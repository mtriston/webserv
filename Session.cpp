//
// Created by mtriston on 30.04.2021.
//

#include "Session.hpp"

Session::  Session(int fd, const Config *config) : _fd(fd), _state(fsm_read), _config(config), _request(), _response() {
  if (fcntl(_fd, F_SETFL, O_NONBLOCK) == - 1) {
    std::cerr << "fcntl error" << std::endl;
  }
}

Session::Session(const Session &x)
    : _fd(x._fd), _state(x._state), _config(x._config), _request(x._request), _response(x._response) {}

Session &Session::operator=(const Session &x) {
  if (this != &x) {
    _fd = x._fd;
    _state = x._state;
    _config = x._config;
    _request = x._request;
    _response = x._response;
  }
  return *this;
}

Session::~Session() {}

Session::Session() {}

int Session::getSocket() const { return _fd; }

fsm_states Session::getState() const { return _state; }

void Session::sendResponse() {
  long ret = write(_fd, _response.data(), _response.size());
  if (ret == -1) {
    std::cerr << "write error" << std::endl;
  } else if (ret < (long)_response.size()) {
    _response = _response.substr(ret, _response.size() - ret);
  } else {
    _response.clear();
    _state = fsm_close;
  }
}

void Session::readRequest() {
  char buf[BUF_SIZE] = {};
  long ret = read(_fd, buf, BUF_SIZE);
  if (ret == -1) {
    std::cerr << "read error" << std::endl;
  }
  _request += buf;
  //TODO: протестить это условие
  if (ret < BUF_SIZE || (_request.size() >= 4 && _request.compare(_request.size() - 4, 5, "\r\n\r\n\0") == 0)) {
    _generateResponse();
    _state = fsm_write;
    _request.clear();
  }
}

void Session::_generateResponse() {
  std::string tmp = cut_next_token(_request, "\r\n\r\n");
  while (!tmp.empty()) {
    Request r(tmp);
    int contentLength = r.getContentLength();
    if (contentLength > 0) {
      r.setBody(_request.substr(0, contentLength));
      _request = _request.substr(contentLength);
    }
    r.print();
    _response += Response(&r, _config).getResponse();
    tmp = cut_next_token(_request, "\r\n\r\n");
  }
}

void Session::closeConnection() const {
  ::close(_fd);
}
