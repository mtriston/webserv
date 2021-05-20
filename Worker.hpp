#include "Server.hpp"

class Worker
{
private:
	IStrategy *strategy_;
public:
	Worker(IStrategy *strategy = 0) : strategy_(strategy) {};

	~Worker() { delete this->strategy_; }

	void setStrategy(IStrategy *strategy) {
		delete this->strategy_;
		this->strategy_ = strategy;
	}

	void doWork() const;
};

class IStrategy {
	public:
	virtual ~IStrategy() {}
	virtual void doWork() = 0;
};

class AcceptConntectionStrategy : public IStrategy {
	private:
	Server *server_;
	public:
	AcceptConntectionStrategy(Server *server) : server_(server) {}
	~AcceptConntectionStrategy() {}
	void doWork() { server_->acceptConnection(); }
};

class ReadRequestStrategy : public IStrategy {
	private:
	Session *session_;
	public:
	ReadRequestStrategy(Session *session) : session_(session) {}
	~ReadRequestStrategy() {}
	void doWork() { session_->readRequest(); }
};

class SendResponseStrategy : public IStrategy {
	private:
	Session *session_;
	public:
	SendResponseStrategy(Session *session) : session_(session) {}
	~SendResponseStrategy() {}
	void doWork() { session_->sendResponse(); }
};

class GenerateResponseStrategy: public IStrategy {
	private:
	Session * session_;
	public:
	GenerateResponseStrategy(Session *session) : session_(session) {}
	~GenerateResponseStrategy() {}
	void doWork() { session_->generateResponse(); }
};