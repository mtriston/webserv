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
  int fd = socket_->acceptConnection();
  if (fd != -1) {
    cluster->addSocket(new ConnectionSocket(fd, socket_->getConfig()));
  } else {
    std::cerr << "Error accepting connection" << std::endl;
  }
  socket_->setIsBusy(false);
}

ReadRequestWork::ReadRequestWork(ConnectionSocket *socket) : socket_(socket) {}

ReadRequestWork::~ReadRequestWork() {}

void ReadRequestWork::doWork(Cluster *cluster) {
  socket_->readRequest();
  if (socket_->getState() == CLOSE_CONNECTION) {
    cluster->removeSocket(socket_);
  } else {
    socket_->setIsBusy(false);
  }
}

SendResponseWork::SendResponseWork(ConnectionSocket *socket) : socket_(socket) {}

SendResponseWork::~SendResponseWork() {}

void SendResponseWork::doWork(Cluster *cluster)  {
  socket_->sendResponse();
  if (socket_->getState() == CLOSE_CONNECTION) {
    cluster->removeSocket(socket_);
  } else {
    socket_->setIsBusy(false);
  }
}

GenerateResponseWork::GenerateResponseWork(ConnectionSocket *socket) : socket_(socket) {}

GenerateResponseWork::~GenerateResponseWork() {}

void GenerateResponseWork::doWork(Cluster *cluster) {
  socket_->generateResponse();
  if (socket_->getState() == CLOSE_CONNECTION) {
    cluster->removeSocket(socket_);
  } else {
    socket_->setIsBusy(false);
  }
}