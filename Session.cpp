//
// Created by mtriston on 30.04.2021.
//

#include "Session.hpp"
#include "IWork.hpp"

Session::Session(int socket, Config *config) : ASocket(socket, config), _state(READ_REQUEST) {}

//Session::Session(const Session &) {}

Session &Session::operator=(const Session &) { return *this; }

//Session::Session() : ASocket() {}

Session::~Session() { close(socket_); }

session_states Session::getState() const { return _state; }

void Session::readRequest() {
  char buffer[BUF_SIZE] = {};
  long wasRead = read(socket_, buffer, BUF_SIZE);
  if (wasRead <= 0) {
    if (wasRead == 0) {
      std::cerr << "Сlient closed the connection" << std::endl;
    } else {
      std::cerr << "Error reading request" << std::endl;
    }
    _state = CLOSE_CONNECTION;
  } else {
    _buffer.append(buffer, wasRead);
    if (_isRequestRead()) {
      _request.parseRequest(_buffer);
      _response.initGenerateResponse(&_request, config_);
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
		long wasSent = write(socket_, _buffer.data(), _buffer.size());
		if (wasSent <= 0) {
    		if (wasSent == 0) {
      			std::cerr << "Сlient closed the connection" << std::endl;
    		} else {
      			std::cerr << "Error sending response" << std::endl;
    		}
        _state = CLOSE_CONNECTION;
		} else {
      _buffer.erase(0, wasSent);
      if (_buffer.empty()) {
        _state = CLOSE_CONNECTION;
      }
    }

}

bool Session::isReadyGenerateResponse(fd_set *readfds, fd_set *writefds) const {
  return _response.isReadyGenerate(readfds, writefds);
}

int Session::fillFdSet(fd_set *readfds, fd_set *writefds) {
  int max_fd = -1;
  if (_state == READ_REQUEST) {
    FD_SET(socket_, readfds);
    max_fd = std::max(max_fd, socket_);
  } else if (_state == GENERATE_RESPONSE) {
    max_fd = std::max(max_fd, _response.fillFdSet(readfds, writefds));
  } else if (_state == SEND_RESPONSE) {
    FD_SET(socket_, writefds);
    max_fd = std::max(max_fd, socket_);
  }
  return max_fd;
}

bool Session::isReady(fd_set *readfds, fd_set *writefds) {
  if (FD_ISSET(socket_, readfds) && _state == READ_REQUEST) {
    return true;
  } else if (FD_ISSET(socket_, writefds) && _state == SEND_RESPONSE) {
    return true;
  } else if (_response.isReadyGenerate(readfds, writefds) && _state == GENERATE_RESPONSE) {
    return true;
  }
  return false;
}

IWork *Session::makeWork() {
  if (_state == READ_REQUEST) {
    return new ReadRequestWork(this);
  } else if (_state == SEND_RESPONSE) {
    return new SendResponseWork(this);
  } else if (_state == GENERATE_RESPONSE) {
    return new GenerateResponseWork(this);
  }
  return 0;
}
