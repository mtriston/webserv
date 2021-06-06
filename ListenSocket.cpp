//
// Created by mtriston on 27.04.2021.
//


#include "ListenSocket.hpp"
#include "IWork.hpp"

ListenSocket::ListenSocket(std::string const &ip, int port, Config_parser *parser) : ASocket(0, port, parser), ip(ip) {}

ListenSocket::~ListenSocket()
{
	close(socket_);
}

int ListenSocket::fillFdSet(fd_set *readfds, fd_set *)
{
	FD_SET(socket_, readfds);
	return socket_;
}

bool ListenSocket::run()
{
	socket_ = socket(AF_INET, SOCK_STREAM, 0);
	if (socket_ == -1) {
		std::cerr << "Error creating socket" << std::endl;
		return false;
	}

	fcntl(socket_, F_SETFL, O_NONBLOCK);

	//Для избежания залипания порта.
	int opt = 1;
	setsockopt(socket_, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

	struct sockaddr_in addr = {};
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port_);
	addr.sin_addr.s_addr = ip.empty() ? htonl(INADDR_ANY) : inet_addr(ip.c_str());

	if (bind(socket_, reinterpret_cast<sockaddr *>(&addr), sizeof(addr)) == -1) {
		std::clog << "Error binding socket" << std::endl;
		return false;
	}
	if (listen(socket_, 16) == -1) {
		std::clog << "Error listening socket" << std::endl;
		return false;
	}
	std::clog << "Listen socket started" << std::endl << std::endl;
	return true;
}

int ListenSocket::acceptConnection()
{

	int cls = accept(socket_, 0, 0);
	if (cls != -1) {
		fcntl(cls, F_SETFL, O_NONBLOCK);
	}
	return cls;
}

bool ListenSocket::isReady(fd_set *readfds, fd_set *)
{
	return !isBusy_ && FD_ISSET(socket_, readfds);
}

IWork *ListenSocket::getWork()
{
	isBusy_ = true;
	return new AcceptConnectionWork(this);
}

ListenSocket::ListenSocket(ListenSocket const &) {}

ListenSocket &ListenSocket::operator=(ListenSocket const &) { return *this; }

ListenSocket::ListenSocket() {}
