#ifndef SERVER_UNIT_H
#define SERVER_UNIT_H
 
#include "Port_unit.hpp"
#include "Parser_unit.hpp"
#include "Working_unit.hpp"
#include <iostream>
#include <cstring>
 
 class Server_unit
 {
	int 					_max_fd;
	fd_set 					_inc_fdset;
	fd_set 					_out_fdset;
	fd_set					_exp_fdset;
	std::list<Request_unit> _recv_wait;
	std::list<Request_unit> _pars_wait;
	std::list<Request_unit> _send_wait;
	std::list<Request_unit> _work_wait;
//	std::list<Port_unit>	_ports_lst;
	Port_unit				_port;
	timeval 				_zero_time;
	Parser_unit 			_parser;
	Working_unit			_work;
	int						_working;
	
	
	void _renew_fd_sets(void);
	void _out_of_circl(void);
	
	void _recv_lst_check(void); //to do rewrt
	void _work_lst_check(void);
	void _pars_lst_check(void);
	void _send_lst_check(void);
	
	public:
		Server_unit(void);
		Server_unit(Server_unit const & for_copy);
		~Server_unit(void);
		Server_unit& operator=(Server_unit const & for_copy);
	
	 	void circule(void);
		void setEnv(char **env);
 };
 
 #endif
