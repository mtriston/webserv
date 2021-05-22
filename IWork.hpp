//
// Created by mtriston on 22.05.2021.
//

#ifndef WEBSERV__IWORK_HPP_
#define WEBSERV__IWORK_HPP_

class ServerCluster;
class Server;
class Session;

class IWork {
 public:
  virtual ~IWork();
  virtual void doWork(ServerCluster *cluster) = 0;
};

class AcceptConntectionWork : public IWork {
 private:
  Server *socket_;
 public:
  AcceptConntectionWork(Server *socket);
  ~AcceptConntectionWork();
  void doWork(ServerCluster *cluster);
};

class ReadRequestWork : public IWork {
 private:
  Session *socket_;
 public:
  ReadRequestWork(Session *socket);
  ~ReadRequestWork();
  void doWork(ServerCluster *cluster);
};

class SendResponseWork : public IWork {
 private:
  Session *socket_;
 public:
  SendResponseWork(Session *socket);
  ~SendResponseWork();
  void doWork(ServerCluster *cluster);
};

class GenerateResponseWork: public IWork {
 private:
  Session *socket_;
 public:
  GenerateResponseWork(Session *socket);
  ~GenerateResponseWork();
  void doWork(ServerCluster *cluster);
};

#endif //WEBSERV__IWORK_HPP_
