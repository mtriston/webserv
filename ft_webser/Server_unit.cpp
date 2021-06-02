#include "Server_unit.hpp"

void Server_unit::_renew_fd_sets(void)
{
	std::list<Request_unit>::iterator it;
	int tmp;
		
	_max_fd = _port.getSocketFD();
	FD_ZERO(&_inc_fdset);
	FD_ZERO(&_out_fdset);
	FD_ZERO(&_exp_fdset);
	FD_SET(_max_fd, &_inc_fdset);
	it = _recv_wait.begin();
	while (it != _recv_wait.end())
	{
		tmp = it->getConnectFD();
		if (_max_fd < tmp)
			_max_fd = tmp;
		FD_SET(tmp, &_inc_fdset);
		++it;
	}
	it = _send_wait.begin();
	while (it != _send_wait.end())
	{
		tmp = it->getConnectFD();
		if (_max_fd < tmp)
			_max_fd = tmp;
		FD_SET(tmp, &_out_fdset);
		++it;
	}
	it = _work_wait.begin();
	while (it != _work_wait.end())
	{
		tmp = it->parsed().read_fd;
		if (_max_fd < tmp)
			_max_fd = tmp;
		if (tmp > -1)
			FD_SET(tmp, &_inc_fdset);
		++it;
	}
	FD_SET(0, &_inc_fdset);//console
	++_max_fd;
}

void Server_unit::_send_lst_check(void)
{
	std::list<Request_unit>::iterator it;
	
	if (_send_wait.empty())
		return ;
	it = _send_wait.begin();
	while (it != _send_wait.end())
	{
		it->sendAnswer(_out_fdset);
		if (it->Status() == DONE)
		{
//to do 	if (it->isComplete()){
			it->disconnect();
			it = _send_wait.erase(it);
std::cout << "send\n";
//			}else{ it->Answer().clear();
//			_work_wait.splice(_work_wait.end(), _send_wait, it);
//			it = _send_wait.begin(); }
		}
		else
			++it;
	}
}

void Server_unit::_work_lst_check(void)
{
	std::list<Request_unit>::iterator it;
	
	if (_work_wait.empty())
		return ;
	it = _work_wait.begin();
	while (it != _work_wait.end())
	{
		_work.work(*it, _inc_fdset);
		if (it->Status() == ANSWER_READY)
		{
std::cout << "ww\n";
			_send_wait.splice(_send_wait.end(), _work_wait, it);
			it = _work_wait.begin();
		}
		else
			++it;
	}
}

void Server_unit::_recv_lst_check(void)
{
	std::list<Request_unit>::iterator it;
	
	if (_recv_wait.empty())
		return ;
	it = _recv_wait.begin();
	while (it != _recv_wait.end())
	{
//		std::cout << "1\n";
		it->receiveLine(_inc_fdset);
//		std::cout << "2\n";
		if (it->Status() == RECV_LINE)
		{
	//		write(1, "1", 1);
			if (it->getReceivedLine().empty())
			{
	//			write(1, "2", 1);
				it->disconnect();
	//			write(1, "3", 1);
				it = _recv_wait.erase(it);
	//			write(1, "4", 1);
			}
			else
			{
std::cout << "recv\n";
	//			write(1, "5", 1);
				_pars_wait.splice(_pars_wait.end(), _recv_wait, it);
	//			write(1, "6", 1);
				it = _recv_wait.begin();
	//			write(1, "7", 1);
			}
	//		write(1, "8\n", 2);
		}
		else
			++it;
	//	write(1, "0", 1);
	}
	//write(1, "out\n", 4);
}

void Server_unit::_pars_lst_check(void)
{
	std::list<Request_unit>::iterator it;
	
	if (_pars_wait.empty())
		return ;
	it = _pars_wait.begin();
	while (it != _pars_wait.end())
	{
		_parser.parse(*it);
		if (it->Status() != RECV_LINE)
		{
std::cout << "parse\n";
			_work_wait.splice(_work_wait.end(), _pars_wait, it);
			it = _pars_wait.begin();
		}
	}
}

Server_unit::Server_unit(void)
{
	try{
		_port.init(80, 1024); //to do rewrt
	}
	catch (Port_unit::exep &ex)
	{
		std::cout << ex.what();
	}
	_working = 1;
}

Server_unit::Server_unit(Server_unit const & for_copy)
{
	*this = for_copy;
}

Server_unit& Server_unit::operator=(Server_unit const & for_copy)
{
	_inc_fdset = for_copy._inc_fdset;
	_out_fdset = for_copy._out_fdset;
	_recv_wait = for_copy._recv_wait;
	_send_wait = for_copy._send_wait;
	_work_wait = for_copy._work_wait;
	_port = for_copy._port;  //to do _ports_lst;
//	_ports_lst = _for_copy._ports_lst;
	_working = for_copy._working;
	return (*this);
}

Server_unit::~Server_unit(void){}


void Server_unit::_out_of_circl(void)
{
	char buf[5];
	read(0, buf, 5);
	buf[4] = '\0';
	if (!strcmp(buf, "stop"))
		_working = 0;
}

void Server_unit::circule(void)
{
//	while (_parser.getStatus())
	while(_working)
	{
		_renew_fd_sets();
		_zero_time.tv_sec = 0;
		_zero_time.tv_usec = 1;
		select(_max_fd, &_inc_fdset, &_out_fdset, &_exp_fdset, &_zero_time); //to do exep set
		_port.checkNewIncome(&_inc_fdset, _recv_wait);
//	write(1, "ma1", 3);
		_recv_lst_check();
//	write(1, "ma2", 3);
		_pars_lst_check();
//	write(1, "ma3", 3);
		_work_lst_check();
//	write(1, "ma4", 3);
		_send_lst_check();
//	write(1, "ma5\n", 4);
		if (FD_ISSET(0, &_inc_fdset))
			_out_of_circl();
	}
	std::cout << _recv_wait.size() << " recv\n";
	std::cout << _work_wait.size() << " work\n";
	std::cout << _send_wait.size() << " send\n";
}

void Server_unit::setEnv(char **env)
{
	_work.setEnv(env);
}