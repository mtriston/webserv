//
// Created by mtriston on 30.05.2021.
//

#ifndef WEBSERV__WORKLIST_HPP_
#define WEBSERV__WORKLIST_HPP_

#include <list>
#include "IWorkList.hpp"
#include "IWork.hpp"

class WorkList : public IWorkList {
private:
	std::list<IWork *> works_;

	WorkList &operator=(WorkList const &);

	WorkList(WorkList const &);

public:
	WorkList();

	~WorkList();

	void addWork(IWork *);

	IWork *getWork();

	bool isEmpty();
};

#endif //WEBSERV__WORKLIST_HPP_
