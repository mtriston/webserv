#include "ASocket.hpp"

ASocket::ASocket(int socket, Config *config) : socket_(socket), config_(config), isBusy_(false) {
}

ASocket::~ASocket() {}

int ASocket::getSocket() const { return socket_; }

Config *ASocket::getConfig() { return config_; }

ASocket::ASocket() {}

ASocket::ASocket(const ASocket &) {}

void ASocket::setIsBusy(bool b) {
 isBusy_ = b;
}
