//
// Created by mtriston on 30.04.2021.
//

#include "Session.hpp"

Session::Session(int fd) : fd(fd), state(fsm_read) {
  fcntl(fd, F_SETFL, O_NONBLOCK);
}

Session::~Session() {}