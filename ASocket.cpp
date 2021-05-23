#include "ASocket.hpp"

ASocket::ASocket(int socket, Config *config) : socket_(socket), config_(config), isBusy_(false) {
  pthread_mutex_init(&mutex_, 0);
}

ASocket::~ASocket() {}

int ASocket::getSocket() const { return socket_; }

Config *ASocket::getConfig() { return config_; }

ASocket::ASocket() {
  pthread_mutex_destroy(&mutex_);
}

ASocket::ASocket(const ASocket &) {}

void ASocket::lockMutex() {
  pthread_mutex_lock(&mutex_);
}

void ASocket::unlockMutex() {
  pthread_mutex_unlock(&mutex_);
}

bool ASocket::isBusy() const {
  return isBusy_;
}

void ASocket::setIsBusy(bool b) {
 isBusy_ = b;
}
