//
// Created by mtriston on 30.04.2021.
//

#ifndef WEBSERV__SESSION_HPP_
#define WEBSERV__SESSION_HPP_
#define BUF_SIZE 1024
#include <fcntl.h>
#include <unistd.h>

enum fsm_states {
  fsm_read,
  fsm_write
};

class Session {
 public:
  Session(int);
  ~Session();

  int fd;
  char buf[BUF_SIZE];
  int buf_used;
  enum fsm_states state;
};

#endif //WEBSERV__SESSION_HPP_
