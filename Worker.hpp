#include "Server.hpp"
#include "Session.hpp"
#include "ServerCluster.hpp"

class IWork {
	public:
	virtual ~IWork() {}
	virtual void doWork(ServerCluster *cluster) = 0;
};

class Worker
{
private:
	IWork *work_;
	ServerCluster *cluster_;
public:
	Worker(IWork *work = 0) : work_(work) {};

	~Worker() { delete this->work_; }

	void setWork(IWork *work) {
		delete this->work_;
		this->work_ = work;
	}

	void doWork() {
		while (1) {
			this->setWork(cluster_->getWork());
			work_->doWork(cluster_);
		}
	}
};

class AcceptConntectionWork : public IWork {
	private:
	Server *socket_;
	public:
	AcceptConntectionWork(Server *socket) : socket_(socket) {}
	~AcceptConntectionWork() {}
	void doWork(ServerCluster *cluster) {
		int fd = socket_->acceptConnection();
		if (fd == -1) {
			cluster->addSocket(new Session(fd, socket_->getConfig()));
		}
	}
};

class ReadRequestWork : public IWork {
	private:
	Session *socket_;
	public:
	ReadRequestWork(Session *socket) : socket_(socket) {}
	~ReadRequestWork() {}
	void doWork(ServerCluster *cluster) {
		socket_->readRequest();
		if (socket_->getState() == CLOSE_CONNECTION) {
			cluster->removeSocket(socket_);
		}
	}
};

class SendResponseWork : public IWork {
	private:
	Session *socket_;
	public:
	SendResponseWork(Session *socket) : socket_(socket) {}
	~SendResponseWork() {}
	void doWork(ServerCluster *cluster)  {
		socket_->sendResponse();
		if (socket_->getState() == CLOSE_CONNECTION) {
			cluster->removeSocket(socket_);
		}
	}
};

class GenerateResponseWork: public IWork {
	private:
	Session *socket_;
	public:
	GenerateResponseWork(Session *socket) : socket_(socket) {}
	~GenerateResponseWork() {}
	void doWork(ServerCluster *cluster) {
		socket_->generateResponse();
		if (socket_->getState() == CLOSE_CONNECTION) {
			cluster->removeSocket(socket_);
		}
	}
};