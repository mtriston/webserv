#include "ASocket.hpp"

	ASocket::ASocket(int socket, Config *config) : socket_(socket), config_(config) {}

	ASocket::~ASocket() {}

	int ASocket::getSocket() { return socket_; }
	
	Config *ASocket::getConfig() { return config_; }