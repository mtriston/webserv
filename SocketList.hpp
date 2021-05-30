//
// Created by mtriston on 30.05.2021.
//

#ifndef WEBSERV__SOCKETLIST_HPP_
#define WEBSERV__SOCKETLIST_HPP_

#include "ISocketList.hpp"
#include <pthread.h>
#include <list>

class SocketList : public ISocketList {
 private:
  std::list<ASocket*> sockets_;

  SocketList(SocketList const &);
  SocketList &operator=(SocketList const &);
 public:
  SocketList();
  ~SocketList();
  void addSocket(ASocket *socket);
  void removeSocket(ASocket *socket);
  std::vector<ASocket*> getSocketArray() const;
};

#endif //WEBSERV__SOCKETLIST_HPP_
