#include "Server.hpp"

class Worker
{
private:
	IWork *Work_;
public:
	Worker(IWork *Work = 0) : Work_(Work) {};

	~Worker() { delete this->Work_; }

	void setWork(IWork *Work) {
		delete this->Work_;
		this->Work_ = Work;
	}

	void doWork() const;
};

class IWork {
	public:
	virtual ~IWork() {}
	virtual void doWork() = 0;
};

class AcceptConntectionWork : public IWork {
	private:
	Server *socket_;
	public:
	AcceptConntectionWork(Server *socket) : socket_(socket) {}
	~AcceptConntectionWork() {}
	void doWork() {
		int cls = ::accept(socket_->getSocket(), 0, 0);
		if (cls == -1) {
			std::cerr << "Error accepting connection" << std::endl;
		} else {
		//	Cluster::instance().addSocket(new ConnectionSocket(cls));
		}
	}
};

class ReadRequestWork : public IWork {
	private:
	Session *socket_;
	public:
	ReadRequestWork(Session *socket) : socket_(socket) {}
	~ReadRequestWork() {}
	void doWork() {
		char buffer[BUF_SIZE] = {};
  		long wasRead = read(socket_->getSocket(), buffer, BUF_SIZE);
  		if (wasRead < 0) {
    		if (wasRead == 0) {
      			std::cerr << "Сlient closed the connection" << std::endl;
    		} else {
      			std::cerr << "Error reading request" << std::endl;
    		}
			//	Cluster::instance().removeSocket(socket_);
		} else {
			socket_->setBuffer(socket_->getBuffer + std::string(buffer, wasRead));
			if (this->_isEndOfReading()) {
			_request.parseRequest(_buffer);
			_response.initGenerateResponse(&_request, _config);
			socket->setHttpResponseGenerator(new HttpResponseGenerator(socket_->getBuffer()));
			socket_->setState(GENERATE_RESPONSE);
    		}
  		}
	}
};

class SendResponseWork : public IWork {
	private:
	Session *socket_;
	public:
	SendResponseWork(Session *socket) : socket_(socket) {}
	~SendResponseWork() {}
	void doWork()  {
		std::string buffer = socket_->getBuffer();
		long wasSent = write(socket_->getSocket(),buffer.data(), buffer.size());
		if (wasSent < 0) {
    		if (wasSent == 0) {
      			std::cerr << "Сlient closed the connection" << std::endl;
    		} else {
      			std::cerr << "Error sending response" << std::endl;
    		}
			//	Cluster::instance().removeSocket(socket_);
		}
		buffer.erase(0, wasSent);
		socket_->setBuffer(buffer);
		if (buffer.empty()) {
			//	Cluster::instance().removeSocket(socket_);
		}
	}
};

class GenerateResponseWork: public IWork {
	private:
	Session * session_;
	public:
	GenerateResponseWork(Session *session) : session_(session) {}
	~GenerateResponseWork() {}
	void doWork() { session_->generateResponse(); }
};