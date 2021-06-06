//
// Created by mtriston on 30.05.2021.
//

#include "WorkList.hpp"

WorkList::WorkList()
{

}

WorkList::~WorkList()
{
	for (std::list<IWork *>::iterator i = works_.begin(); i != works_.end(); ++i) {
		delete *i;
	}
}

void WorkList::addWork(IWork *work)
{
	works_.push_back(work);
}

IWork *WorkList::getWork()
{
	IWork *work = works_.front();
	works_.pop_front();
	return work;
}

bool WorkList::isEmpty()
{
	return works_.empty();
}
