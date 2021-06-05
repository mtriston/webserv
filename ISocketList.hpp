//
// Created by mtriston on 30.05.2021.
//

#ifndef WEBSERV__ISOCKETLIST_HPP_
#define WEBSERV__ISOCKETLIST_HPP_

#include <vector>

class ASocket;

class ISocketList {
public:
	virtual ~ISocketList() {}

	virtual void addSocket(ASocket *) = 0;

	virtual void removeSocket(ASocket *) = 0;
};

#endif //WEBSERV__ISOCKETLIST_HPP_
