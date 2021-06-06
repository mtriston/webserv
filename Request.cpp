//
// Created by mtriston on 01.05.2021.
//

#include "Request.hpp"

Request::Request() {}

Request::Request(const Request &x) : _headers(x._headers) {}

Request::~Request() {}

void Request::parseRequest(std::string request)
{
	parseFirstLine(request);
	parseHeaders(request);
	parseBody(request);
}

std::string const &Request::getMethod() { return _headers["method"]; }

std::string const &Request::getPath() { return _headers["path"]; }

std::string const &Request::getVersion() { return _headers["version"]; }

void Request::parseFirstLine(std::string &request)
{
	std::string line = cutToken(request, "\r\n");
	_headers["method"] = cutToken(line, " ");
	_headers["path"] = cutToken(line, " ");
	_headers["version"] = cutToken(line, " ");
}

void Request::parseHeaders(std::string &request)
{
	for (std::string l = cutToken(request, "\r\n"); !l.empty(); l = cutToken(request, "\r\n")) {
		std::string first = cutToken(l, ": ");
		_headers.insert(std::make_pair(ft_tolower(first), l));
	}
	request.erase(0, 2);
}
//TODO: Обработать POST запрос

int Request::getContentLength()
{
	return std::atoi(_headers["content-length"].c_str());
}

std::string Request::getHost() const
{
	std::map<std::string, std::string>::const_iterator x = _headers.find("host");
	if (x == _headers.end()) {
		return "";
	} else {
		return x->second;
	}
}

void Request::parseBody(std::string &request)
{
	_headers["body"] = "";
	if (_headers["transfer-encoding"] == "chunked") {
		long size = 0;
		char* p_end;
		while (!request.empty()) {
			size = std::strtol(cutToken(request, "\r\n").c_str(), &p_end, 16);
			_headers["body"] += request.substr(0, size);
			request.erase(0, size + 2);
		}
	} else {
		_headers["body"] = request.substr(0, getContentLength());
	}
}

std::string const &Request::getBody()
{
	return _headers["body"];
}
