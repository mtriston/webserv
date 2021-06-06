//
// Created by mtriston on 30.05.2021.
//

#ifndef WEBSERV__IWORKLIST_HPP_
#define WEBSERV__IWORKLIST_HPP_

class IWork;

class IWorkList {
public:
	virtual ~IWorkList() {}

	virtual IWork *getWork() = 0;

	virtual bool isEmpty() = 0;
};

#endif //WEBSERV__IWORKLIST_HPP_
