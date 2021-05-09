#include "Response.hpp"

Response::Response() {}

Response::Response(Request *request, const Config *config) : _request(request), _config(config) {}

Response::Response(Response const &x) : _request(x._request), _config(x._config), _response(x._response) {}

Response::~Response() {}

void Response::generateResponse() {
  if (_request->getMethod() == "GET")
    _handleMethodGET();
}

void Response::_handleMethodGET() {
  std::stringstream headers;
  time_t *t;
  time(t);

  _readContent();
  _analyzeContent();
  headers << "HTTP/1.1 " << _content.status << "\r\n";
  headers << "Content-Length: " << _content.contentLength << "\r\n";
  headers << "Content-Type: " << _content.contentType << "\r\n";
  headers << "Date: " << convertTime(t) << "\r\n";
  headers << "Last-Modified: " << _content.lastModified << "\r\n";
  headers << "Server: " << "webserv21" << "\r\n"; //_config.getServerName()
  headers << "\r\n";
  _response = headers.str() + _content.data;
}

std::string const &Response::getResponse() { return _response; }

void Response::_readContent() {
   _content.file = _config->getRoot() + _request->getPath();
  if (_request->getPath() == "/")
    _content.file += _config->getIndex().front();

  std::ifstream fin;
  fin.open(_content.file.c_str());
  if (!fin.is_open()) {
    if (errno == EACCES) {
      _content.status = "403";
      _content.file = "/home/mtriston/CLionProjects/webserv/site/error_pages/404.html";
    } else if (errno == ENOENT) {
      _content.status = "404";
      _content.file = "/home/mtriston/CLionProjects/webserv/site/error_pages/404.html";
    }
    fin.open(_content.file.c_str());
  }
  std::stringstream buff;
  buff << fin.rdbuf();
  _content.data = buff.str();
}

std::string Response::_getContentType(const std::string &file) {
  std::string result;
  if (!file.empty()) {
    if (file.size() > 5 && file.compare(file.size() - 5, 5, ".html") == 0) {
      result = "text/html";
    } else if (file.size() > 4 && file.compare(file.size() - 4, 4, ".jpg") == 0) {
      result = "image/jpeg";
    }  else if (file.size() > 4 && file.compare(file.size() - 4, 4, ".png") == 0) {
      result = "image/png";
    } else {
      result = "text/plain";
    }
  }
  return result;
}

void Response::_analyzeContent() {
  struct stat info = {};
  stat(_content.file.c_str(), &info);
  char *tmp = ft_itoa(info.st_size);
  _content.contentLength = std::string(tmp);
  free(tmp);
  _content.contentType = _getContentType(_content.file);
  _content.lastModified = convertTime(&info.st_mtime);
  std::cout << _content.lastModified << std::endl;
}
