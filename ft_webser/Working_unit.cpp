#include "Working_unit.hpp"

 
void Working_unit::_fileRead(Request_unit &Request, fd_set &_inc_set)
{
	if (!FD_ISSET(Request.parsed().read_fd, &_inc_set))
		return;

	char	buf[10241];
	int		len = 10240;

	len = read(Request.parsed().read_fd, buf, 10240);
	Request.Answer().insert(Request.Answer().end(), buf, &buf[len]);
	if (len == 10240)
		return;
	close(Request.parsed().read_fd);
	Request.Status() = ANSWER_READY;
	return;
}

void Working_unit::_CGI_read(Request_unit &Request, fd_set &_inc_set)
{
	if (!FD_ISSET(Request.parsed().read_fd, &_inc_set))
		return;
	char	buf[10241];
	int		len = 10240;
	int		status;
	int		wp;

	len = read(Request.parsed().read_fd, buf, 10240);
	Request.Answer().insert(Request.Answer().end(), buf, &buf[len]);
	if (len == 10240)
		return;
	wp = waitpid(Request.parsed().pid, &status, WNOHANG);
	if (wp == 0) //to do rewr
		return;
	if (!WIFEXITED(status))
	{
		std::string err = "execve error";
		Request.Answer().assign(err.begin(), err.end());
	}
	Request.Status() = ANSWER_READY;
	close(Request.parsed().read_fd);
}


void Working_unit::_cgi_child(int pipes[2], char const * name)
{
	std::string full_path = "sev/";
	char **argv = NULL;
	close(pipes[0]);
	dup2(pipes[1], 1);
	full_path += name;
	execve(full_path.c_str(), argv, _env);//to do exeptions
	exit(1);
}

void Working_unit::_fork_CGI(Request_unit &Request)
{
	int pipes[2];
	pipe(pipes);
	Request.parsed().read_fd = pipes[0];
	fcntl(pipes[0], F_SETFL, O_NONBLOCK);
	Request.parsed().pid = fork();
	if (Request.parsed().pid == 0)
		_cgi_child(pipes, Request.getReceivedLine().c_str());
	else if (Request.parsed().pid > 0)
		close(pipes[1]);
	Request.Status() = CGI_WORK;
}

void Working_unit::work(Request_unit &Request, fd_set &_inc_set)
{
//	std::cout << "work\n";
//	std::cout << &Request << "\n";
	if (Request.Status() == PARSED)
		_test(Request);
		//_open_file(Request);
	else if (Request.Status() == CGI)
		_fork_CGI(Request);
	else if (Request.Status() == IN_WORK)
		_fileRead(Request, _inc_set);
	else if (Request.Status() == CGI_WORK)
		_CGI_read(Request, _inc_set);
	else if (Request.Status() == SAVE_FILE)
		_create_file(Request);
	else if (Request.Status() == SAVE_WORK)
		_write_file(Request);
	else if (Request.Status() == ANSWER_READY)
		return;
}

void Working_unit::_open_file(Request_unit &Request)
{
	//write(1, "of\n", 3);
	std::string full_path = "sev/"; //to do something
	full_path += Request.getReceivedLine();
	Request.parsed().read_fd = open(full_path.c_str(), O_RDONLY, S_IROTH);
	if (Request.parsed().read_fd == -1)//заглушка для страниц с ошибками
	{
		full_path += " can't open file1\n";
		Request.Answer().assign(full_path.begin(), full_path.end());
		Request.Status() = ANSWER_READY;
	}
	else
		Request.Status() = IN_WORK;
}

void Working_unit::_create_file(Request_unit &Request)
{
	static int i = 48;
	std::string full_path = "sev/test"; //to do something
	full_path += i++;
	Request.parsed().read_fd = \
					open(full_path.c_str(), O_WRONLY | O_CREAT, S_IROTH);
	if (Request.parsed().read_fd == -1)//заглушка для страниц с ошибками
	{
		full_path += " can't open file\n";
		Request.Answer().assign(full_path.begin(), full_path.end());
		Request.Status() = ANSWER_READY;
	}
	else
		Request.Status() = SAVE_WORK;
}

void Working_unit::_write_file(Request_unit &Request)
{
	
	std::list<std::string>::iterator it;
	it = Request.parsed()._splited_rl.begin();
	char u = '0';
		
	write(Request.parsed().read_fd,
		Request.getReceivedLine().c_str(),
			Request.getReceivedLine().size());
//	std::cout << Request.Status() << " WF\n";
	while (it != Request.parsed()._splited_rl.end())
	{
		std::cout << *it << std::endl;
		write(Request.parsed().read_fd, &u, 1);
		++u;
		write(Request.parsed().read_fd, " ", 1);
		write(Request.parsed().read_fd, 
			(*it).c_str(), (*it).size());
		write(Request.parsed().read_fd, "\n", 1);
		++it;
	}
	

	close(Request.parsed().read_fd);
	std::string temp = "Saved file\n";
	Request.Answer().assign(temp.begin(), temp.end());
	Request.Status() = ANSWER_READY;
}

Working_unit::Working_unit(void){}
Working_unit::Working_unit(Working_unit const & for_copy){}
Working_unit& Working_unit::operator=(Working_unit const & for_copy)
{
	return (*this);
}
Working_unit::~Working_unit(void){}
void Working_unit::setEnv(char** env)
{
	_env = env;
}


void Working_unit::_test(Request_unit& Request)
{
	Parsered_unit &Pars = Request.parsed();
	std::cout << "\n-------------------\nPars\n";
	std::cout << "Body_len: " << Pars.c_length_uli << "\n";
	std::cout << "\nBody:\n" << Pars.body << "\n";
	if (!Pars.host.name.empty() || Pars.host.error)
	{
		std::cout << "\nHost:\nType: " << Pars.host.type << "\n";
		std::cout << "Name: " << Pars.host.name << "\n";
		std::cout << "Addr: ";
		for (int i = 0; i < 4; i++)
			std::cout << (int)Pars.host.addr[i] << " ";
		std::cout << "\nPort: " << Pars.host.port << "\n";
		std::cout << "Error: " << Pars.host.error << "\n";
		 
	}
	std::cout << "\n--------------------\n";
	Request.Status() = ANSWER_READY;
}