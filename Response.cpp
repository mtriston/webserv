#include "Response.hpp"

Response::Response() {}

Response::Response(Request const &request, Config const &config) : request_(request), config_(config) {}

Response::Response(Response const &x) : request_(x.request_), config_(x.config_), headers_(x.headers_), response_(x.response_) {}

Response::~Response() {}

std::string const &Response::getResponse() {
	response_ = request_.getVersion() + "200 OK\r\n";
	response_ += "Server: webserv\r\n";
	response_ += "Date: now\r\n";
	response_ += "Content-Type: text/html\r\n";
	response_ += "Content-Length: 22\r\n";
	response_ += "Keep-Alive: timeout=25\r\n";
	response_ += "\r\n";
	response_ += "<h1>Hello, World!</h1>";
	return response_;	
}

