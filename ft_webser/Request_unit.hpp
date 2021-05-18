#ifndef REQUEST_UNIT_H
#define REQUEST_UNIT_H

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
#include <vector>
#include <strings.h>
#include <cstring>
#include <map>

#include <iostream>
#include <stdio.h>

#define JUST_CREATED 0
#define RECV_LINE 1
#define PARSED 2
#define IN_WORK 3
#define ANSWER_READY 4
#define ANSWER_SENDING 5
#define DONE 6
#define CGI 7
#define CGI_WORK 8
#define SAVE_FILE 9
#define SAVE_WORK 10
#define COR_EMTY 11


//method defines
#define GET 1
#define POST 2
#define PUT 3
#define HEAD 4

struct Date_unit
{
	std::string day_name;
	char		day_name_int;
	char		day;
	std::string mounth_name;
	char		mounth_int;
	char 		year;
	char 		hour;
	char 		minute;
	char 		second;
	bool 		error;
	bool 		empty;
};

struct User_agent
{
	std::string product_str;
	char		product_ver[2];
	std::string comment_str;
	bool 		empty;
	bool 		error;
	std::string information_str;
	std::string platform_str;
	std::string details_str;
	std::string extensions;
};

struct Auth_unit
{
	std::string type;
	std::string realm;
	std::string charset;
	bool 		error;	
};

struct Host_unit
{
	char type;
	std::string name;
	unsigned char addr[4];
	int port;
	bool error;	
};

struct Parsered_unit
{
	bool					is_cgi;
	int						read_fd;
	pid_t 					pid;
	std::list<std::string>  _splited_rl;
	int						method;
	std::string 			path;
	int 					error;
	int						port;
	int						prot_ver[2];
	std::string   			body;
	char const *			body_pt;
	
	//headers
	Host_unit   			host;
	std::multimap\
	<int,	std::string>	a_charset_mmp;
	std::multimap\
	<int,	std::string>	a_language_mmp;
	std::multimap\
	<int,	std::string>	c_language_mmp;
	size_t					c_length_uli;
	std::multimap\
	<int,	std::string>	c_location_mmp;
	std::multimap\
	<int,	std::string>	c_type_mmp;
	std::multimap\
	<int,	std::string>	allow_mmp; //to do перепиши в лист
	std::pair<std::string,\
	 std::string>			authorization_pr;	
	Date_unit 				date_sc;
//	Date_unit 				last_mod; //to do это запихни в ответ
//	location    //to do тоже в ответ
	std::string 			referer_str;
	//Retry-After// to do ответ
	//Server
	//Transfer-Encoding
	User_agent 				user_agent_s;
	Auth_unit				www_auth;
};

class Request_unit
{
	struct sockaddr_in	_addr;
	int					_ConnectFD;
	int					_buffer_size;
	std::string			_received_line;
	std::vector<char>	_answer;
	Parsered_unit		_parsed;
	int					_status;

	public:
		Request_unit(void);
		Request_unit(Request_unit const & for_copy);
		~Request_unit(void);
		Request_unit& operator= (Request_unit const& for_copy);
		
		void init(int ConnectFD, int buffer_size, int _port);
		void disconnect(void);
		
		void receiveLine(fd_set &_recv_set);
		void sendAnswer(fd_set &_send_set);
		
		std::vector<char>& Answer(void);
		
		sockaddr_in *getAddr_ptr(void);
		int getConnectFD(void);
		std::string &getReceivedLine(void);
		Parsered_unit &parsed(void);
		int &Status(void);
};
/*
////to do вставь нормально
int	ft_atoi(const char *str)
{
	size_t	num;
	int		sign;
	int		i;

	num = 0;
	i = 0;
	sign = 1;
	if (str[0] == 0)
		return (num);
	while ((8 < str[i] && str[i] < 14) || str[i] == 32)
		i++;
	(str[i] == 45) ? (sign = -1) : (sign);
	(str[i] == 45 || str[i] == 43) ? (i++) : (i);
	while (str[i] > 47 && str[i] < 58)
	{
		num = num * 10 + (str[i] - 48);
		if ((num) == 9223372036854775807)
			return ((sign == -1) ? (int)(1) : (int)(-1));
		if (num > 9223372036854775807)
			return ((sign == -1) ? (int)(0) : (int)(-1));
		i++;
	}
	return ((int)(num * sign));
}*/

#endif
