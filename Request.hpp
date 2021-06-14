//
// Created by mtriston on 01.05.2021.
//

#ifndef WEBSERV__REQUEST_HPP_
#define WEBSERV__REQUEST_HPP_

#include <string>
#include <map>
#include <cstdlib>
#include "utils.hpp"

class Request {
public:
	Request();

	Request(Request const &);

	~Request();

	std::string const &getMethod();

	std::string getPath();

	std::string getPathInfo();

	std::string getQueryString();

	std::string const &getVersion();

	std::string const &getBody();

	size_t getContentLength();

	std::string getContentType();

	std::string getHost() const;

	std::string getAuthType();

	std::string getAccept();

	std::string getReferer();

	std::string getUserAgent();

	std::string getCookies();

	bool parseRequest(std::string request);

private:
	Request &operator=(Request const &);

	void parseFirstLine(std::string &);

	void parseHeaders(std::string &);

	bool parseBody(std::string &);

	std::map<std::string, std::string> _headers;
};

#endif //WEBSERV__REQUEST_HPP_
