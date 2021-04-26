//
// Created by mtriston on 26.04.2021.
//

#ifndef WEBSERV__WEBSERVER_HPP_
#define WEBSERV__WEBSERVER_HPP_
#include <iostream>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

//TODO: заменить define на данные из когфиг файла
#define PORT 4444
#define QLEN 16
#define IP "127.0.0.1"

class WebServer {
 public:
  static WebServer& Instance();
  void setup();
  void run();
  void finish();

 private:

  int ls; // listening socket;
  struct sockaddr_in addr; // ls's addr

  WebServer();
  WebServer(WebServer const &);
  WebServer& operator=(WebServer const &);
};

#endif //WEBSERV__WEBSERVER_HPP_
