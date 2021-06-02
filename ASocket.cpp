#include "ASocket.hpp"

ASocket::ASocket(int socket) : socket_(socket), isBusy_(false) {}

ASocket::~ASocket() {}

int ASocket::getSocket() const { return socket_; }

ASocket::ASocket() {}

ASocket::ASocket(const ASocket &) {}

void ASocket::setIsBusy(bool b) {
 isBusy_ = b;
}
