//
// Created by mtriston on 30.04.2021.
//

#include "Session.hpp"

Session::Session(int fd) : _fd(fd), _buf(), _state(fsm_read), _request(), _response("HTTP/1.1 200 OK\n\n<h1>Hello</h1>\n") {
  memset(_buf, 0, BUF_SIZE);
  if (fcntl(_fd, F_SETFL, O_NONBLOCK) == - 1) {
    std::cerr << "fcntl error" << std::endl;
  }
}

Session::Session(const Session &x)
    : _fd(x._fd), _buf(), _state(x._state), _request(x._request), _response(x._response) {
  strncpy(_buf, x._buf, BUF_SIZE);
}
int Session::get_socket() const { return _fd; }

fsm_states Session::get_state() const { return _state; }

void Session::send_response() {
  long ret = write(_fd, _response.c_str(), _response.length());
  if (ret < _response.length()) {
    _response = _response.substr(ret, _response.length() - ret);
  } else {
    _response.clear();
  }
}

void Session::read_request() {
  long ret = read(_fd, _buf, BUF_SIZE);
  if (ret == -1 || ret == 0) {
    std::cerr << "read error" << std::endl;
  }
  _request += _buf;
  memset(_buf, 0, BUF_SIZE);
  if (_request.length() >= 4 && _request.compare(_request.length() - 4, 5, "\r\n\r\n\0") == 0) {
    _state = fsm_write;
    //TODO: Сформировать http ответ
    std::cout << _request << std::endl;
    //TODO: Удалить распечатку запроса
    _request.clear();
  }
}

void Session::close() const {
  ::close(_fd);
}

Session::~Session() {}

Session::Session() {}
