//
// Created by mtriston on 30.04.2021.
//

#include "Session.hpp"

Session::  Session(int fd, const Config *config) : _fd(fd), _state(READ_REQUEST), _config(config), _buffer() {
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
    _state = CLOSE_CONNECTION;
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
    _request.parseRequest(_buffer);
    _buffer.clear();
    generateResponse();
    _state = SEND_RESPONSE;
  }
}

void Session::generateResponse() {
    Response tempResponse(&_request, _config);
    tempResponse.generateResponse();
  _buffer = tempResponse.getResponse();
}

void Session::closeConnection() const {
  ::close(_fd);
}
