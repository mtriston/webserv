//
// Created by mtriston on 22.05.2021.
//

#ifndef WEBSERV__IWORK_HPP_
#define WEBSERV__IWORK_HPP_

class WorkerManager;

class ListenSocket;

class ConnectionSocket;

class IWork {
public:
	virtual ~IWork();

	virtual void doWork(WorkerManager *) = 0;
};

class AcceptConnectionWork : public IWork {
private:
	ListenSocket *socket_;
public:
	explicit AcceptConnectionWork(ListenSocket *socket);

	~AcceptConnectionWork();

	void doWork(WorkerManager *);
};

class ReadRequestWork : public IWork {
private:
	ConnectionSocket *socket_;
public:
	explicit ReadRequestWork(ConnectionSocket *socket);

	~ReadRequestWork();

	void doWork(WorkerManager *);
};

class SendResponseWork : public IWork {
private:
	ConnectionSocket *socket_;
public:
	explicit SendResponseWork(ConnectionSocket *socket);

	~SendResponseWork();

	void doWork(WorkerManager *);
};

class GenerateResponseWork : public IWork {
private:
	ConnectionSocket *socket_;
public:
	explicit GenerateResponseWork(ConnectionSocket *socket);

	~GenerateResponseWork();

	void doWork(WorkerManager *);
};

#endif //WEBSERV__IWORK_HPP_
