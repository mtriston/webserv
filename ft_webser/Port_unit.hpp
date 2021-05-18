 
#ifndef PORT_UNIT_H
#define PORT_UNIT_H

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <string>
#include <exception>
#include <fcntl.h>
#include <list>
#include <strings.h>
#include "Request_unit.hpp"

class Port_unit
{
	int 				_port;
	int 				_SocketFD;
	int 				_buffer_size;
	bool 				_ready;
	struct sockaddr_in	_addr;
	
	public:
		class exep : std::exception
		{
			std::string _what;

			public:
				exep(char const *what){_what += what;}; //to do uberi v fail
				virtual char const* what(void){return (_what.c_str());}; //to do-/-
				virtual ~exep(){};	
		};
		Port_unit(void);
		Port_unit(Port_unit const &for_copy);
		~Port_unit(void);
		void init(int port, int buffer_size);
		bool checkReady(void) const;
		void checkNewIncome(fd_set *set, std::list<Request_unit>& recv_waiting);
		
		Port_unit& operator= (Port_unit const & for_copy);
		
		int getSocketFD(void);
};

#endif