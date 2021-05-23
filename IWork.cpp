//
// Created by mtriston on 22.05.2021.
//

#include "IWork.hpp"
#include "Cluster.hpp"
#include "ListenSocket.hpp"
#include "ConnectionSocket.hpp"

IWork::~IWork() {}


AcceptConnectionWork::AcceptConnectionWork(ListenSocket *socket) : socket_(socket) {}

AcceptConnectionWork::~AcceptConnectionWork() {}

void AcceptConnectionWork::doWork(Cluster *cluster) {
  socket_->lockMutex();
  int fd = socket_->acceptConnection();
  if (fd != -1) {
    std::cout <<"Connection was accepted" << std::endl;
    cluster->addSocket(new ConnectionSocket(fd, socket_->getConfig()));
  } else {
    std::cerr << "Error accepting connection" << std::endl;
  }
  socket_->setIsBusy(false);
  socket_->unlockMutex();
}

ReadRequestWork::ReadRequestWork(ConnectionSocket *socket) : socket_(socket) {}

ReadRequestWork::~ReadRequestWork() {}

void ReadRequestWork::doWork(Cluster *cluster) {
  socket_->lockMutex();
  std::cout << "Reading request..." << std::endl;
  socket_->readRequest();
  if (socket_->getState() == CLOSE_CONNECTION) {
    cluster->removeSocket(socket_);
  } else {
    socket_->setIsBusy(false);
    socket_->unlockMutex();
  }
}

SendResponseWork::SendResponseWork(ConnectionSocket *socket) : socket_(socket) {}

SendResponseWork::~SendResponseWork() {}

void SendResponseWork::doWork(Cluster *cluster)  {
  socket_->lockMutex();
  socket_->sendResponse();
  std::cout << "Sending response..." << std::endl;
  if (socket_->getState() == CLOSE_CONNECTION) {
    cluster->removeSocket(socket_);
  } else {
    socket_->setIsBusy(false);
    socket_->unlockMutex();
  }
}

GenerateResponseWork::GenerateResponseWork(ConnectionSocket *socket) : socket_(socket) {}

GenerateResponseWork::~GenerateResponseWork() {}

void GenerateResponseWork::doWork(Cluster *cluster) {
  socket_->lockMutex();
  socket_->generateResponse();
  std::cout << "Generate response..." << std::endl;
  if (socket_->getState() == CLOSE_CONNECTION) {
    cluster->removeSocket(socket_);
  } else {
    socket_->setIsBusy(false);
    socket_->unlockMutex();
  }
}
