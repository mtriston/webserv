#include "Response.hpp"

Response::Response() {}

Response::Response(Request *request, const Config *config) : request_(request), config_(config) {}

Response::Response(Response const &x) : request_(x.request_), config_(x.config_), response_(x.response_) {}

Response::~Response() {}

std::string const &Response::getResponse() {
  std::string file = config_->getRoot() + request_->getPath();
  if (request_->getPath() == "/") {
    file += config_->getIndex().front();
  }
  std::basic_string<char> content = _getContent(file);
  response_ = _getStatusLine();
//  response_ += "Server: webserv\r\n";
//  response_ += "Date: now\r\n";
  response_ += _getContentLength(file);
  response_ += _getContentType(file);
  response_ += "\r\n";
  response_ += content;
  return response_;
}

std::string Response::_getStatusLine() {
  return "HTTP/1.1 " + _getStatusCode() + "\r\n";
}

std::string Response::_getStatusCode() {
  return "200 OK";
}

std::string Response::_getContent(std::string const &file) {
  std::basic_string<char> content;
  long ret = 1;
  int fd = open(file.c_str(), O_RDONLY);
  while (ret > 0) {
    char buf[1024] = {};
    ret = read(fd, buf, 1024);
    if (ret > 0) {
      content += std::string(buf, ret);
    }
  }
  close(fd);
  return content;
}

std::string Response::_getContentLength(const std::string &file) {
  std::string result;
  if (!file.empty()) {
    struct stat info = {};
    ::stat(file.c_str(), &info);
    char *len = ft_itoa(info.st_size);
    result = "content-length: " + std::string(len) + "\r\n";
    free(len);
  }
  return result;
}

std::string Response::_getContentType(const std::string &file) {
  std::string result;
  if (!file.empty()) {
    if (file.size() > 5 && file.compare(file.size() - 5, 5, ".html") == 0) {
      result = "content-type: text/html\r\n";
    } else if (file.size() > 4 && file.compare(file.size() - 4, 4, ".jpg") == 0) {
      result = "content-type: image/jpeg\r\n";
    }  else if (file.size() > 4 && file.compare(file.size() - 4, 4, ".png") == 0) {
      result = "content-type: image/png\r\n";
    } else {
      result = "content-type: text/plain\r\n";
    }
  }
  return result;
}
