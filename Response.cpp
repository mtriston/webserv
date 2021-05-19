#include "Response.hpp"

Response::Response() : _request(0), _config(0), _response(),  fd(0), _state(GENERATE_HEADERS) {}

Response::Response(Response const &x) : _request(x._request), _config(x._config), _response(x._response), fd(x.fd), _state(x._state) {}

Response::~Response() {}


int Response::getFd() const { return fd; }

void Response::initGenerateResponse(Request *request, const Config *config) {
  _request = request;
  _config = config;
  if (_request->getMethod() == "GET") {
    _handleMethodGET();
  } else if (_request->getMethod() == "HEAD") {
    _handleMethodHEAD();
  } else if (_request->getMethod() == "POST") {

  }
}

void Response::generateResponse() {
  if (_state == READ_FILE) {
    char buf[100024] = {};
    long ret = read(fd, buf, 100024);
    if (ret < 0) { std::cerr << "read error" << std::endl; }
    else { _content.data.append(std::string(buf, 1024)); }
   //if (_content.data.size() == (size_t)std::atoi(_content.contentLength.c_str())) {
      _response += _content.data;
      _state = READY_FOR_SEND;
      close(fd);
  }
}

void Response::_handleMethodHEAD() {
  std::stringstream headers;
  time_t t;
  time(&t);

  _readContent();
  headers << "HTTP/1.1 " << _content.status << "\r\n";
  headers << "Content-Length: " << _content.contentLength << "\r\n";
  headers << "Content-Type: " << _content.contentType << "\r\n";
  headers << "Date: " << convertTime(&t) << "\r\n";
  headers << "Last-Modified: " << _content.lastModified << "\r\n";
  headers << "Server: " << "webserv21" << "\r\n"; //_config.getServerName()
  headers << "\r\n";
  _response = headers.str();
  close(fd);
  _state = READY_FOR_SEND;
}


void Response::_handleMethodGET() {
  std::stringstream headers;
  time_t t;
  time(&t);

  _readContent();
  headers << "HTTP/1.1 " << _content.status << "\r\n";
  headers << "Server: " << "webserv21" << "\r\n"; //_config.getServerName()
  headers << "Date: " << convertTime(&t) << "\r\n";
  headers << "Content-Type: " << _content.contentType << "\r\n";
  headers << "Content-Length: " << _content.contentLength << "\r\n";
  headers << "Last-Modified: " << _content.lastModified << "\r\n";
  headers << "\r\n";
  _response = headers.str();
  _state = READ_FILE;
}

std::string const &Response::getResponse() const { return _response; }

bool Response::isGenerated() const { return _state == READY_FOR_SEND; }

bool Response::isNeedToRead() const { return _state != WRITE_FILE; }

void Response::_readContent() {
   _content.file = _config->getRoot() + _request->getPath();
  if (_request->getPath() == "/")
    _content.file += _config->getIndex().front();

  fd = open(_content.file.c_str(), O_RDONLY);
  _content.status = "200";
  if (fd == -1) {
    if (errno == EACCES) {
      _content.status = "403";
      _content.file = "/home/mtriston/CLionProjects/webserv/site/error_pages/404.html";
    } else if (errno == ENOENT) {
      _content.status = "404";
      _content.file = "/home/mtriston/CLionProjects/webserv/site/error_pages/404.html";
    }
    fd = open(_content.file.c_str(), O_RDONLY);
  }
  struct stat info = {};
  stat(_content.file.c_str(), &info);
  char *tmp = ft_itoa(info.st_size);
  _content.contentLength = std::string(tmp);
  free(tmp);
  _content.contentType = _getContentType(_content.file);
  _content.lastModified = convertTime(&info.st_mtime);
  fcntl(fd, F_SETFL, O_NONBLOCK);
}

std::string Response::_getContentType(const std::string &file) {
    if (file.size() > 5 && file.compare(file.size() - 5, 5, ".html") == 0) {
      return "text/html";
    } else if (file.size() > 4 && file.compare(file.size() - 4, 4, ".jpg") == 0) {
      return "image/jpeg";
    }  else if (file.size() > 4 && file.compare(file.size() - 4, 4, ".png") == 0) {
      return "image/png";
    } else if (file.size() > 4 && file.compare(file.size() - 4, 4, ".css") == 0) {
      return "text/css";
    } else if (file.size() > 3 && file.compare(file.size() - 4, 4, ".js") == 0) {
      return "application/javascript";
    } else {
      return "text/plain";
    }
}