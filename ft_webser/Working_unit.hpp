#ifndef WORKING_UNIT_H
#define WORKING_UNIT_H

#include "Request_unit.hpp"
#include <sys/types.h>
#include <sys/wait.h>

class Working_unit
{
	void _fileRead(Request_unit &Request, fd_set &_inc_set);
	void _CGI_read(Request_unit &Request, fd_set &_inc_set);
	void _open_file(Request_unit &Request);
	void _fork_CGI(Request_unit &Request);
	void _cgi_child(int pipes[2], char const * name);
	char **_env;
	
	void _create_file(Request_unit& Request);
	void _write_file(Request_unit& Request);
	
	void _test(Request_unit& Request);
	
	public:
		void setEnv(char** env);
		Working_unit(void);
		Working_unit(Working_unit const & for_copy);
		Working_unit& operator=(Working_unit const & for_copy);
		~Working_unit(void);
		
		
		
		void work(Request_unit &Request, fd_set &_inc_set);
};

#endif
