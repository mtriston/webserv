//
// Created by mtriston on 30.04.2021.
//

#ifndef WEBSERV__CONNECTIONSOCKET_HPP_
#define WEBSERV__CONNECTIONSOCKET_HPP_

class Config_parser;

#include <string>
#include <sys/types.h>
#include <unistd.h>
#include <iostream>
#include "ASocket.hpp"
#include "Response.hpp"
#include "Request.hpp"

enum session_states {
	READ_REQUEST,
	GENERATE_RESPONSE,
	SEND_RESPONSE,
	CLOSE_CONNECTION
};

class ConnectionSocket : public ASocket {
public:
	ConnectionSocket(int socket, int port, Config_parser *parser);

	~ConnectionSocket();

	session_states getState() const;

	void readRequest();

	void generateResponse();

	const std::string &getBuffer() const;

	void sendResponse();

	bool _isRequestRead();

	int fillFdSet(fd_set *readfds, fd_set *writefds);

	bool isReady(fd_set *readfds, fd_set *writefds);

	IWork *getWork();

private:
	ConnectionSocket();

	ConnectionSocket(ConnectionSocket const &);

	ConnectionSocket &operator=(ConnectionSocket const &);

	enum session_states _state;
	std::string _buffer;
	Response *_response;
};

#endif //WEBSERV__CONNECTIONSOCKET_HPP_
