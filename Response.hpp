//
// Created by mtriston on 04.05.2021.
//

#ifndef WEBSERV__RESPONSE_HPP_
#define WEBSERV__RESPONSE_HPP_

#include <string>
#include <sys/types.h>

class ConnectionSocket;

class config_unit;

class Request;

struct response_data {
public:
	int fd;
	std::string file;
	std::string content;
	int contentLength;
	std::string contentType;
	std::string lastModified;
	int status;
};

enum response_states {
	PREPARE_FOR_GENERATE,
	READ_FILE,
	READ_CGI,
	WRITE_FILE,
	READY_FOR_SEND
};

class Response {
public:
	Response(ConnectionSocket *);

	~Response();

	int fillFdSet(fd_set *readfds, fd_set *writefds) const;

	void initGenerateResponse();

	void generateResponse();

	std::string getResponse() const;

	bool isReadyGenerate(fd_set *readfds, fd_set *writefds) const;

	bool isGenerated() const;

private:
	Response();

	Response(Response const &);

	Response &operator=(Response const &);

	void _handleMethodGET();

	void _handleMethodDELETE();

	void _handleNotAllowedMethod();

	void _openContent();

	void _writeContent();

	std::string getHeaders() const;

	std::string _getContentType(std::string const &);

	ConnectionSocket *socket;
	config_unit *config;
	Request *request;
	struct response_data responseData_;
	response_states state_;
};

#endif //WEBSERV__RESPONSE_HPP_
