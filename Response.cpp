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

  _readContent();
  headers << "HTTP/1.1 " << _status << "\r\n";
  headers << "Content-Lenght: " << _response.size() << "\r\n";
  headers << "Content-Type: " << _mimeType << "\r\n";
  headers << "\r\n";
  _response = headers.str() + _response;
}

std::string const &Response::getResponse() { return _response; }

void Response::_readContent() {
  std::string file_name;
  std::ifstream fin;
  std::stringstream buff;

  file_name = _config->getRoot() + _request->getPath();
  if (_request->getPath() == "/")
    file_name += _config->getIndex().front();

  fin.open(file_name.c_str());
  std::cerr << "Error: " << strerror(errno);
  if (fin.is_open()) {
    buff << fin.rdbuf();
    _status = 200;
  } else {
    file_name = "/home/mtriston/CLionProjects/webserv/site/error_pages/404.html";
    fin.open(file_name.c_str());
    buff << fin.rdbuf();
    _status = 404;
  }
  fin.close();
  _response = buff.str();
  _mimeType = _getContentType(file_name);
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