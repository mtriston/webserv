//
// Created by mtriston on 01.05.2021.
//

#include "Request.hpp"

Request::Request() {}

Request::Request(const Request &x) : _headers(x._headers) {}

Request::~Request() {}

bool Request::parseRequest(std::string request)
{
	parseFirstLine(request);
	parseHeaders(request);
	return parseBody(request);
}

std::string const &Request::getMethod() { return _headers["method"]; }

std::string Request::getPath()
{
	std::string tmp = _headers["path"];
	size_t queryPos = tmp.find('?');
	if (queryPos != std::string::npos)
		tmp.erase(queryPos);
	return tmp;
}

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
		std::string first = ft_trim(cutToken(l, ":"));
		_headers.insert(std::make_pair(ft_tolower(first), ft_trim(l)));
	}
	request.erase(0, 2);
}

size_t Request::getContentLength()
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

bool Request::parseBody(std::string &request)
{
	_headers["body"] = "";
	if (_headers["transfer-encoding"] == "chunked") {
		long size = 0;
		while (!request.empty()) {
			size = scanNumber(cutToken(request, "\r\n").c_str(), 16);
			if (size < 0 || size > (long) request.size() + 2) {
				return false;
			}
			_headers["body"] += request.substr(0, size);
			request.erase(0, size + 2);
		}
	} else {
		if (getContentLength() > request.size()) {
			return false;
		}
		_headers["body"] = request.substr(0, getContentLength());
	}
	return true;
}

std::string const &Request::getBody()
{
	return _headers["body"];
}

std::string Request::getQueryString()
{
	std::string tmp = _headers["path"];
	size_t queryPos = tmp.find('?');
	if (queryPos != std::string::npos)
		return tmp.substr(tmp.find('?') + 1);
	return "";
}

std::string Request::getAuthType()
{
	std::string tmp = _headers["authorization"];
	return tmp.substr(0, tmp.find(' '));
}

std::string Request::getContentType()
{
	return _headers["content-type"];
}

std::string Request::getAccept()
{
	return _headers["accept"];
}

std::string Request::getReferer()
{
	return _headers["referer"];
}

std::string Request::getUserAgent()
{
	return _headers["user-agent"];
}

std::string Request::getCookies()
{
	return _headers["cookie"];
}

std::string Request::getPathInfo()
{
	return getPath();
}
