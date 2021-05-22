//
// Created by mtriston on 22.05.2021.
//

#include "IWork.hpp"
#include "ServerCluster.hpp"
#include "Server.hpp"
#include "Session.hpp"

IWork::~IWork() {}


AcceptConntectionWork::AcceptConntectionWork(Server *socket) : socket_(socket) {}

AcceptConntectionWork::~AcceptConntectionWork() {}

void AcceptConntectionWork::doWork(ServerCluster *cluster) {
  int fd = socket_->acceptConnection();
  if (fd != -1) {
    cluster->addSocket(new Session(fd, socket_->getConfig()));
  }
}

ReadRequestWork::ReadRequestWork(Session *socket) : socket_(socket) {}

ReadRequestWork::~ReadRequestWork() {}

void ReadRequestWork::doWork(ServerCluster *cluster) {
  socket_->readRequest();
  if (socket_->getState() == CLOSE_CONNECTION) {
    cluster->removeSocket(socket_);
  }
}

SendResponseWork::SendResponseWork(Session *socket) : socket_(socket) {}

SendResponseWork::~SendResponseWork() {}

void SendResponseWork::doWork(ServerCluster *cluster)  {
  socket_->sendResponse();
  if (socket_->getState() == CLOSE_CONNECTION) {
    cluster->removeSocket(socket_);
  }
}

GenerateResponseWork::GenerateResponseWork(Session *socket) : socket_(socket) {}

GenerateResponseWork::~GenerateResponseWork() {}

void GenerateResponseWork::doWork(ServerCluster *cluster) {
  socket_->generateResponse();
  if (socket_->getState() == CLOSE_CONNECTION) {
    cluster->removeSocket(socket_);
  }
}