#include "ASocket.hpp"

ASocket::ASocket(int socket, int port, Config_parser *config)
{
	this->socket_ = socket;
	this->port_ = port;
	this->config_ = config;
	this->isBusy_ = false;
}

ASocket::~ASocket() {}

int ASocket::getSocket() const { return socket_; }

ASocket::ASocket() {}

ASocket::ASocket(const ASocket &) {}

void ASocket::setIsBusy(bool b)
{
	isBusy_ = b;
}

int ASocket::getPort() const
{
	return port_;
}

Config_parser *ASocket::getConfig() const
{
	return config_;
}
