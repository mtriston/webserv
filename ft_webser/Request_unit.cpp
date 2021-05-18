#include "Request_unit.hpp" 

Request_unit::Request_unit(void)
{
	bzero(&_addr, sizeof(_addr)); //to do ft_bzero
	_ConnectFD = -1;
	_buffer_size = -1;
	_status = -1;
	_parsed.c_length_uli = -1;
	_parsed.date_sc.empty = 1;
	_parsed.user_agent_s.empty = 1;
}

Request_unit::Request_unit(Request_unit const &for_copy)
{
	*this = for_copy;
}

Request_unit::~Request_unit(void){}

Request_unit& Request_unit::operator= (Request_unit const & for_copy)
{
	_ConnectFD = for_copy._ConnectFD;
	_addr = for_copy._addr;
	_buffer_size = for_copy._buffer_size;
	_received_line = for_copy._received_line;
	_answer = for_copy._answer;
	_status = for_copy._status;
	return (*this);
}

void Request_unit::init(int ConnectFD, int buffer_size, int port)
{
	_ConnectFD = ConnectFD;
	_buffer_size = buffer_size;
	_parsed.port = port;
}

void Request_unit::disconnect(void)
{
	close(_ConnectFD);
}

void Request_unit::receiveLine(fd_set &_recv_set)
{
	if (!FD_ISSET(_ConnectFD, &_recv_set))
		return;

	int			len;
	size_t		body_size;
	char		buf[_buffer_size + 1];
	char const	*size_pt;
//	char const	*body_pt;
	
	len = recv(_ConnectFD, buf, _buffer_size, MSG_WAITALL);
	if (len < 1)
		return;
	buf[len] = '\0';
//std::cout << len << "   " << buf << "\n";
	_received_line += buf;
	if (!(_parsed.body_pt = strstr(_received_line.c_str(), "\r\n\r\n")))
			return;
	if (!(size_pt = strstr(_received_line.c_str(), "Content-Length:")))
		_status = RECV_LINE;
	else
	{
		_parsed.c_length_uli = atoi(&size_pt[15]);
//		std::cout << len << " len\n--";
		
		_parsed.body_pt += 4;
//		std::cout << body_pt << " body\n";
		body_size = strlen(_parsed.body_pt);
		if (_parsed.c_length_uli + 2 > body_size)
			return ;
		_status = RECV_LINE;
	}
	std::cout << _received_line << std::endl;
}

void Request_unit::sendAnswer(fd_set &_send_set)
{
	int len;
	if (!FD_ISSET(_ConnectFD, &_send_set))
		return;
	len = send(_ConnectFD, &_answer[0], _answer.size(), MSG_WAITALL);//to do w8
	if (len < 1)
		return;
	else if (len < _answer.size())
	{
		std::vector<char>::iterator it = _answer.begin() + len;
		_answer.erase(_answer.begin(), it);
		_status = ANSWER_SENDING;
	}
	else
		_status = DONE;
	return;
}

int &Request_unit::Status(void)
{
	return (_status);
}

std::vector<char>& Request_unit::Answer(void)
{
	return (_answer);
}
		
sockaddr_in *Request_unit::getAddr_ptr(void)
{
	return (&_addr);
}

int Request_unit::getConnectFD(void)
{
	return (_ConnectFD);
}

std::string & Request_unit::getReceivedLine(void)
{
	return (_received_line);
}

Parsered_unit &Request_unit::parsed(void)
{
	return (_parsed);
}