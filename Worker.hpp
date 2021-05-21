#ifndef WORKER_HPP
#define WORKER_HPP

#include "ServerCluster.hpp"
class ServerCluster;
class IWork;
class Server;
class Session;

class IWork {
	public:
	virtual ~IWork();
	virtual void doWork(ServerCluster *cluster) = 0;
};

class Worker
{
private:
	IWork *work_;
	ServerCluster *cluster_;
public:
	Worker(IWork *work = 0);
	~Worker();
	void setWork(IWork *work);
	void doWork();
};

class AcceptConntectionWork : public IWork {
	private:
	Server *socket_;
	public:
	AcceptConntectionWork(Server *socket);
	~AcceptConntectionWork();
	void doWork(ServerCluster *cluster);
};

class ReadRequestWork : public IWork {
	private:
	Session *socket_;
	public:
	ReadRequestWork(Session *socket);
	~ReadRequestWork();
	void doWork(ServerCluster *cluster);
};

class SendResponseWork : public IWork {
	private:
	Session *socket_;
	public:
	SendResponseWork(Session *socket);
	~SendResponseWork();
	void doWork(ServerCluster *cluster);
};

class GenerateResponseWork: public IWork {
	private:
	Session *socket_;
	public:
	GenerateResponseWork(Session *socket);
	~GenerateResponseWork();
	void doWork(ServerCluster *cluster);
};

#endif