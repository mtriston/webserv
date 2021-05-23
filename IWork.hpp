//
// Created by mtriston on 22.05.2021.
//

#ifndef WEBSERV__IWORK_HPP_
#define WEBSERV__IWORK_HPP_

class Cluster;
class ListenSocket;
class ConnectionSocket;

class IWork {
 public:
  virtual ~IWork();
  virtual void doWork(Cluster *cluster) = 0;
};

class AcceptConnectionWork : public IWork {
 private:
  ListenSocket *socket_;
 public:
  explicit AcceptConnectionWork(ListenSocket *socket);
  ~AcceptConnectionWork();
  void doWork(Cluster *cluster);
};

class ReadRequestWork : public IWork {
 private:
  ConnectionSocket *socket_;
 public:
  explicit ReadRequestWork(ConnectionSocket *socket);
  ~ReadRequestWork();
  void doWork(Cluster *cluster);
};

class SendResponseWork : public IWork {
 private:
  ConnectionSocket *socket_;
 public:
  explicit SendResponseWork(ConnectionSocket *socket);
  ~SendResponseWork();
  void doWork(Cluster *cluster);
};

class GenerateResponseWork: public IWork {
 private:
  ConnectionSocket *socket_;
 public:
  explicit GenerateResponseWork(ConnectionSocket *socket);
  ~GenerateResponseWork();
  void doWork(Cluster *cluster);
};

#endif //WEBSERV__IWORK_HPP_
