//
// Created by mtriston on 30.05.2021.
//

#include "SocketList.hpp"
#include "ASocket.hpp"

SocketList::SocketList()
{
}

SocketList::~SocketList()
{
	for (std::list<ASocket *>::iterator i = connectSockets_.begin(); i != connectSockets_.end(); ++i) {
		delete *i;
	}
	for (std::vector<ASocket *>::iterator i = listenSockets_.begin(); i != listenSockets_.end(); ++i) {
		delete *i;
	}
}

void SocketList::addSocket(ASocket *socket)
{
	connectSockets_.push_back(socket);
}

void SocketList::removeSocket(ASocket *socket)
{
	connectSockets_.remove(socket);
	delete socket;
}

std::vector<ASocket *> SocketList::getSocketArray() const
{
	std::vector<ASocket *> array(connectSockets_.begin(), connectSockets_.end());
	array.insert(array.begin(), listenSockets_.begin(), listenSockets_.end());
	return array;
}

void SocketList::addListenSocket(ASocket *socket)
{
	listenSockets_.push_back(socket);
}

void SocketList::resetAllConnections()
{
	for (std::list<ASocket *>::iterator i = connectSockets_.begin(); i != connectSockets_.end(); ++i) {
		delete *i;
	}
	connectSockets_.clear();
}