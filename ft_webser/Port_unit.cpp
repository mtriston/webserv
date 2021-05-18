#include "Port_unit.hpp"

Port_unit::Port_unit(void)
{
	_ready = false;
	_SocketFD = -1;
	bzero(&_addr, sizeof(_addr)); //to do bzero
}

Port_unit::Port_unit(Port_unit const & for_copy)
{
	*this = for_copy;
}

Port_unit::~Port_unit(void){
	close(_SocketFD);
}

void Port_unit::checkNewIncome(fd_set *set,\
				std::list<Request_unit>& recv_waiting)
{	
	if (!FD_ISSET(_SocketFD, set))
		return ;
		
	Request_unit temp;
	socklen_t addr_len = sizeof(*(temp.getAddr_ptr()));
	int ConnectFD;
	
	ConnectFD = accept(_SocketFD, (sockaddr*)temp.getAddr_ptr(), &addr_len);
	if (ConnectFD == -1)
		return ;

	temp.init(ConnectFD, _buffer_size, _port);		
	recv_waiting.push_back(temp);
	fcntl(ConnectFD, F_SETFL, O_NONBLOCK);
}

int Port_unit::getSocketFD(void)
{
	return (_SocketFD);
}

void Port_unit::init(int port, int buffer_size)
{
	int tmp = 1;

	_port = port;
	_buffer_size = buffer_size;
	_SocketFD = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (_SocketFD == -1)
		throw exep("prep"); //todo norm exep
	_addr.sin_family = PF_INET;
	_addr.sin_port = htons(_port);
	_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	setsockopt(_SocketFD, SOL_SOCKET, SO_REUSEADDR, &tmp, sizeof(int));
	if (bind(_SocketFD, (struct sockaddr *) &_addr, sizeof(_addr)) == -1)
		throw exep("bind"); //to do norm exep
	if (listen(_SocketFD, 10) == -1)
		throw exep("Listen"); //to do norm
	fcntl(_SocketFD, F_SETFL, O_NONBLOCK);
}

bool Port_unit::checkReady(void) const
{
	return (_ready);
}

Port_unit& Port_unit::operator= (Port_unit const & for_copy)
{
	_SocketFD = for_copy._SocketFD;
	_port = for_copy._port;
	_buffer_size = for_copy._buffer_size;
	_ready = for_copy._ready;
	_addr = for_copy._addr;
	return (*this);
}