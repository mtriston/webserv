//
// Created by mtriston on 30.04.2021.
//

#include "Session.hpp"

Session::  Session(int fd, const Config *config) : _fd(fd), _state(fsm_read), _config(config), _buffer() {
  if (fcntl(_fd, F_SETFL, O_NONBLOCK) == - 1) {
    std::cerr << "fcntl error" << std::endl;
  }
}

Session::Session(const Session &x)
    : _fd(x._fd), _state(x._state), _config(x._config), _buffer(x._buffer) {}

Session &Session::operator=(const Session &x) {
  if (this != &x) {
    _fd = x._fd;
    _state = x._state;
    _config = x._config;
    _buffer = x._buffer;
  }
  return *this;
}

Session::~Session() {}

Session::Session() {}

int Session::getSocket() const { return _fd; }

fsm_states Session::getState() const { return _state; }

void Session::sendResponse() {
  long ret = write(_fd, _buffer.data(), _buffer.size());
  if (ret == -1) {
    std::cerr << "write error" << std::endl;
  } else if (ret < (long)_buffer.size()) {
    _buffer = _buffer.substr(ret, _buffer.size() - ret);
  } else {
    _buffer.clear();
    _state = fsm_close;
  }
}

void Session::readRequest() {
  char buf[BUF_SIZE] = {};
  long ret = read(_fd, buf, BUF_SIZE);
  if (ret == -1) {
    std::cerr << "read error" << std::endl;
  }
  _buffer += buf;
  //TODO: протестить это условие
  if (ret < BUF_SIZE || (_buffer.size() >= 4 && _buffer.compare(_buffer.size() - 4, 5, "\r\n\r\n\0") == 0)) {
    _generateResponse();
    _state = fsm_write;
  }
}

void Session::_generateResponse() {
  std::string response;
  std::string tmp = cut_next_token(_buffer, "\r\n\r\n");
  while (!tmp.empty()) {
    Request tempRequest(tmp);
    int contentLength = tempRequest.getContentLength();
    if (contentLength > 0) {
      tempRequest.setBody(_buffer.substr(0, contentLength));
      _buffer = _buffer.substr(contentLength);
    }
    tempRequest.print();
    Response tempResponse(&tempRequest, _config);
    tempResponse.generateResponse();
    response += tempResponse.getResponse();
    tmp = cut_next_token(_buffer, "\r\n\r\n");
  }
  _buffer = response;
}

void Session::closeConnection() const {
  ::close(_fd);
}
