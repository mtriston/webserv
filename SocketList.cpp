//
// Created by mtriston on 30.05.2021.
//

#include "SocketList.hpp"
#include "ASocket.hpp"

void SocketList::addSocket(ASocket *socket) {
  sockets_.push_back(socket);
}

void SocketList::removeSocket(ASocket *socket) {
  sockets_.remove(socket);
  delete socket;
}

SocketList::~SocketList() {
  for (std::list<ASocket*>::iterator i = sockets_.begin(); i != sockets_.end(); ++i) {
    delete *i;
  }
}

SocketList::SocketList() {
}

std::vector<ASocket*> SocketList::getSocketArray() const {
  return std::vector<ASocket*>(sockets_.begin(), sockets_.end());
}
