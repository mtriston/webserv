//
// Created by mtriston on 30.04.2021.
//

#include "Session.hpp"

Session::Session(int fd, const Config *config) : _fd(fd), _state(READ_REQUEST), _config(config) {
  if (fcntl(_fd, F_SETFL, O_NONBLOCK) == -1) {
    std::cerr << "fcntl error" << std::endl;
  }
}

Session::Session(const Session &x)
    : _fd(x._fd),
      _state(x._state),
      _buffer(x._buffer),
      _request(x._request),
      _response(x._response),
      _config(x._config) {}

Session &Session::operator=(const Session &) { return *this; }

Session::Session() : _fd(), _state(), _buffer(), _request(), _response(), _config() {}

Session::~Session() {}

int Session::getSocket() const { return _fd; }

session_states Session::getState() const { return _state; }

void Session::readRequest() {
  char buffer[BUF_SIZE] = {};
  long wasRead = read(_fd, buffer, BUF_SIZE);
  if (wasRead < 0) {
    _state = CLOSE_CONNECTION;
    if (wasRead == 0) {
      std::cerr << "Сlient closed the connection" << std::endl;
    } else {
      std::cerr << "Error reading request" << std::endl;
    }
  } else {
    _buffer.append(buffer, wasRead);
    if (_isRequestRead()) {
      _request.parseRequest(_buffer);
      _response.initGenerateResponse(&_request, _config);
      _state = GENERATE_RESPONSE;
    }
  }
}

bool Session::_isRequestRead() {
  unsigned long headerEndPos = _buffer.find("\r\n\r\n");
  if (headerEndPos != std::string::npos) {
    unsigned long contentLengthPos = _buffer.find("Content-Length:");
    if (contentLengthPos != std::string::npos && contentLengthPos < headerEndPos) {
      if (_buffer.size() == headerEndPos + 4 + std::atoi(_buffer.c_str() + contentLengthPos + 15)) {
        return true;
      }
      return false;
    }
    unsigned long transferEncodingPos = _buffer.find("Transfer-Encoding: chunked");
    if (transferEncodingPos != std::string::npos && transferEncodingPos < headerEndPos) {
      if (_buffer.compare(_buffer.size() - 5, 5, "0\r\n\r\n") == 0) {
        return true;
      }
      return false;
    }
    return true;
  }
  return false;
}

void Session::generateResponse() {
  _response.generateResponse();
  if (_response.isGenerated()) {
    _buffer = _response.getResponse();
    _state = SEND_RESPONSE;
  }
}

void Session::sendResponse() {
  long ret = write(_fd, _buffer.data(), _buffer.size());
  if (ret == -1) {
    std::cerr << "write error" << std::endl;
  } else if (ret < (long) _buffer.size()) {
    _buffer = _buffer.substr(ret, _buffer.size() - ret);
  } else {
    _buffer.clear();
    _state = CLOSE_CONNECTION;
  }
}

void Session::closeConnection() const {
  ::close(_fd);
}

bool Session::isReadyGenerateResponse(fd_set *readfds, fd_set *writefds) const {
  return _response.isReadyGenerate(readfds, writefds);
}

int Session::fillFdSet(fd_set *readfds, fd_set *writefds) const {
  int max_fd = -1;
  if (_state == READ_REQUEST) {
    FD_SET(_fd, readfds);
    max_fd = std::max(max_fd, _fd);
  } else if (_state == GENERATE_RESPONSE) {
    max_fd = std::max(max_fd, _response.fillFdSet(readfds, writefds));
  } else if (_state == SEND_RESPONSE) {
    FD_SET(_fd, writefds);
    max_fd = std::max(max_fd, _fd);
  }
  return max_fd;
}
