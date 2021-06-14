//
// Created by mtriston on 30.04.2021.
//

#include "ConnectionSocket.hpp"
#include "IWork.hpp"
#include "Config_parser.hpp"

#define BUF_SIZE 8096
#define MAX_BODY 15000000

ConnectionSocket::ConnectionSocket(int socket, int port, Config_parser *parser)
		: ASocket(socket, port, parser), _state(READ_REQUEST) {
	_response = new Response(this);
	contentLength = -1;
	isChunked = false;
	headerEndPos = std::string::npos;
	bodySize = 0;
}

ConnectionSocket::ConnectionSocket(const ConnectionSocket &) {}

ConnectionSocket &ConnectionSocket::operator=(const ConnectionSocket &) { return *this; }

ConnectionSocket::ConnectionSocket() {}

ConnectionSocket::~ConnectionSocket()
{
	delete _response;
	close(socket_);
}

session_states ConnectionSocket::getState() const { return _state; }

void ConnectionSocket::readRequest()
{
	char buffer[BUF_SIZE] = {};
	long wasRead = read(socket_, buffer, BUF_SIZE);
	if (wasRead <= 0) {
		if (wasRead == 0) {
			std::cerr << "Сlient closed the connection" << std::endl;
		} else {
			std::cerr << "Error reading request" << std::endl;
		}
		_state = CLOSE_CONNECTION;
	} else {
		buf.push_back(std::string(buffer, wasRead));
		if (_isRequestRead()) {
			_buffer.reserve(bodySize + headerEndPos + 4);
			for (std::list<std::string>::iterator i = buf.begin(); i != buf.end(); ++i) {
				_buffer.append(*i);
			}
			_response->initGenerateResponse();
			_state = GENERATE_RESPONSE;
			if (_response->isGenerated()) {
				_buffer = _response->getResponse();
				_state = SEND_RESPONSE;
			}
		}
	}
}

bool ConnectionSocket::_isRequestRead()
{
	if (headerEndPos == std::string::npos) {
		headerEndPos = buf.back().find("\r\n\r\n");
		if (headerEndPos != std::string::npos) {
			bodySize = buf.back().size() - headerEndPos - 4;
			if (bodySize == 0)
				return true;
		}
	} else {
		bodySize += buf.back().size();
	}

	if (!isChunked && contentLength < 0) {
		checkTransferEncodingHeader() || checkContentLengthHeader();
	}

	if (contentLength >= 0 && contentLength >= (long)bodySize) {
		return true;
	}

	if (isChunked) {
		std::string tmp = buf.back();
		if (buf.back().size() < 5) {
			std::list<std::string>::iterator preLast = buf.end();
			std::advance(preLast, -2);
			tmp += *preLast;
		}
		if (tmp.compare(tmp.size() - 5, 5, "0\r\n\r\n") == 0)
			return true;
	}
	return false;
}

void ConnectionSocket::generateResponse()
{
	_response->generateResponse();
	if (_response->isGenerated()) {
		_buffer = _response->getResponse();
		_state = SEND_RESPONSE;
	}
}

void ConnectionSocket::sendResponse()
{
	long wasSent = write(socket_, _buffer.data(), _buffer.size());
	if (wasSent <= 0) {
		if (wasSent == 0) {
			std::cerr << "Сlient closed the connection" << std::endl;
		} else {
			std::cerr << "Error sending response" << std::endl;
		}
		_state = CLOSE_CONNECTION;
	} else {
		_buffer.erase(0, (size_t) wasSent);
		if (_buffer.empty()) {
			_state = CLOSE_CONNECTION;
		}
	}

}

int ConnectionSocket::fillFdSet(fd_set *readfds, fd_set *writefds)
{
	int max_fd = -1;
	if (_state == READ_REQUEST) {
		FD_SET(socket_, readfds);
		max_fd = std::max(max_fd, socket_);
	} else if (_state == GENERATE_RESPONSE) {
		max_fd = std::max(max_fd, _response->fillFdSet(readfds, writefds));
	} else if (_state == SEND_RESPONSE) {
		FD_SET(socket_, writefds);
		max_fd = std::max(max_fd, socket_);
	}
	return max_fd;
}

bool ConnectionSocket::isReady(fd_set *readfds, fd_set *writefds)
{
	if (isBusy_) return false;
	if (FD_ISSET(socket_, readfds) && _state == READ_REQUEST) {
		return true;
	} else if (FD_ISSET(socket_, writefds) && _state == SEND_RESPONSE) {
		return true;
	} else if (_response->isReadyGenerate(readfds, writefds) && _state == GENERATE_RESPONSE) {
		return true;
	}
	return false;
}

IWork *ConnectionSocket::getWork()
{
	isBusy_ = true;
	if (_state == READ_REQUEST) {
		return new ReadRequestWork(this);
	} else if (_state == SEND_RESPONSE) {
		return new SendResponseWork(this);
	} else if (_state == GENERATE_RESPONSE) {
		return new GenerateResponseWork(this);
	}
	std::cerr << "RETURN NULL IN GETWORK!!!" << std::endl;
	return 0;
}

const std::string &ConnectionSocket::getBuffer() const
{
	return _buffer;
}

bool ConnectionSocket::checkContentLengthHeader()
{
	unsigned long contentLengthPos = ft_tolower(buf.back()).find("content-length:");
	if (contentLengthPos != std::string::npos && contentLengthPos < headerEndPos) {
		contentLength = scanNumber(_buffer.c_str() + contentLengthPos + 15, 10);
		return true;
	}
	return false;
}

bool ConnectionSocket::checkTransferEncodingHeader()
{
	unsigned long transferEncodingPos = ft_tolower(buf.back()).find("transfer-encoding: chunked");
	if (transferEncodingPos != std::string::npos && transferEncodingPos < headerEndPos) {
		isChunked = true;
		return true;
	}
	return false;
}