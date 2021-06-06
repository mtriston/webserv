//
// Created by mtriston on 22.05.2021.
//

#include "IWork.hpp"
#include "WorkerManager.hpp"
#include "ListenSocket.hpp"
#include "ConnectionSocket.hpp"

IWork::~IWork() {}

AcceptConnectionWork::AcceptConnectionWork(ListenSocket *socket) : socket_(socket) {}

AcceptConnectionWork::~AcceptConnectionWork() {}

void AcceptConnectionWork::doWork(WorkerManager *workerManager)
{
	int fd = socket_->acceptConnection();
	if (fd != -1) {
		workerManager->addSocket(new ConnectionSocket(fd, socket_->getPort(), socket_->getConfig()));
	} else {
		std::cerr << "Error accepting connection" << std::endl;
	}
	socket_->setIsBusy(false);
}

ReadRequestWork::ReadRequestWork(ConnectionSocket *socket) : socket_(socket) {}

ReadRequestWork::~ReadRequestWork() {}

void ReadRequestWork::doWork(WorkerManager *workerManager)
{
	socket_->readRequest();
	if (socket_->getState() == CLOSE_CONNECTION) {
		workerManager->removeSocket(socket_);
	} else {
		socket_->setIsBusy(false);
	}
}

SendResponseWork::SendResponseWork(ConnectionSocket *socket) : socket_(socket) {}

SendResponseWork::~SendResponseWork() {}

void SendResponseWork::doWork(WorkerManager *workerManager)
{
	socket_->sendResponse();
	if (socket_->getState() == CLOSE_CONNECTION) {
		workerManager->removeSocket(socket_);
	} else {
		socket_->setIsBusy(false);
	}
}

GenerateResponseWork::GenerateResponseWork(ConnectionSocket *socket) : socket_(socket) {}

GenerateResponseWork::~GenerateResponseWork() {}

void GenerateResponseWork::doWork(WorkerManager *workerManager)
{
	socket_->generateResponse();
	if (socket_->getState() == CLOSE_CONNECTION) {
		workerManager->removeSocket(socket_);
	} else {
		socket_->setIsBusy(false);
	}
}