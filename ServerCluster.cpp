//
// Created by mtriston on 26.04.2021.
//

#include "ServerCluster.hpp"

ServerCluster &ServerCluster::Instance() {
  static ServerCluster theSingleInstance;
  return theSingleInstance;
}

void ServerCluster::setup() {
  //TODO: цикл со всеми серверами
  Server server(IP, PORT);
  _servers.insert(std::make_pair(server.getLS(), server));
  Server server1(IP2, PORT);
  _servers.insert(std::make_pair(server1.getLS(), server1));
}

void ServerCluster::run() {
  //TODO: Реализовать функционал.
  while (1) {
    fd_set readfds, writefds;
    int max_d = _servers.rbegin()->first;
    FD_ZERO(&readfds);
    FD_ZERO(&writefds);
    std::map<int, Server>::iterator begin = _servers.begin();
    std::map<int, Server>::iterator end = _servers.end();
    while (begin != end) {
      FD_SET(begin->first, &readfds);
      max_d = max_d < begin->first ? begin->first : max_d;
      std::map<int, Session>::iterator cli_begin = begin->second.sessions.begin();
      std::map<int, Session>::iterator cli_end = begin->second.sessions.end();
      while (cli_begin != cli_end) {
        FD_SET(cli_begin->first, &readfds);
        if (cli_begin->second.state == fsm_write) {
          FD_SET(cli_begin->first, &writefds);
        }
        max_d = max_d < cli_begin->first ? cli_begin->first : max_d;
        cli_begin++;
      }
      ++begin;
    }
    int res = select(max_d + 1, &readfds, &writefds, 0, 0);
    if (res == -1) {
      std::cerr << "Select error" << std::endl; //TODO: ???
      continue;
    } else if (res == 0) {
      continue; // тайм аут
    }

    {
      std::map<int, Server>::iterator begin = _servers.begin();
      std::map<int, Server>::iterator end = _servers.end();
      while (begin != end) {
        if (FD_ISSET(begin->first, &readfds)) {
          begin->second.accept();
        }
        ++begin;
      }
    }

    {
      std::map<int, Server>::iterator begin = _servers.begin();
      std::map<int, Server>::iterator end = _servers.end();
      while (begin != end) {
        std::map<int, Session>::iterator cli_begin = begin->second.sessions.begin();
        std::map<int, Session>::iterator cli_end = begin->second.sessions.end();
        while (cli_begin != cli_end) {
          if (FD_ISSET(cli_begin->first, &readfds)) {
            read(cli_begin->first, cli_begin->second.buf, BUF_SIZE);
            cli_begin->second.state = fsm_write;
            std::cout << cli_begin->second.buf << std::endl;
          }
          ++cli_begin;
        }
        ++begin;
      }
    }

    {
      std::map<int, Server>::iterator begin = _servers.begin();
      std::map<int, Server>::iterator end = _servers.end();
      while (begin != end) {
        std::map<int, Session>::iterator cli_begin = begin->second.sessions.begin();
        std::map<int, Session>::iterator cli_end = begin->second.sessions.end();
        while (cli_begin != cli_end) {
          if (FD_ISSET(cli_begin->first, &writefds)) {
            std::string answer = "HTTP/1.1 200 OK\n\n<h1>Hello</h1>\n";
            write(cli_begin->first, answer.c_str(), answer.length());
            int tmp = cli_begin->first;
            ++cli_begin;
            begin->second.closeSession(tmp);
          } else {
            ++cli_begin;
          }
        }
        ++begin;
      }
    }
    ++begin;
  }
}

void ServerCluster::finish() {
  std::map<int, Server>::iterator begin = _servers.begin();
  std::map<int, Server>::iterator end = _servers.end();
  while (begin != end) {
    begin->second.finish();
  }
}

ServerCluster::ServerCluster() {}

ServerCluster::ServerCluster(ServerCluster const &x) { (void) x; }

ServerCluster &ServerCluster::operator=(ServerCluster const &x) {
  (void) x;
  return *this;
}
