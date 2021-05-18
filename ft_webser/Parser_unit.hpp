#ifndef PARSER_UNIT_H
#define PARSER_UNIT_H

#include "Request_unit.hpp"

class Parser_unit
{
	bool _work;
	void _crlf_split(Request_unit &Request);
	void _crlf_spliting(int *borders, char const *str);
	void _first_line(int border, char const* str, Parsered_unit &Pars);
	void _char_split(char const*str, int *borders, char ch, int stop);
	bool _str_step(char const *, int &, int, char, bool);
	
	
	//header funcs
	void _h_host(Parsered_unit &Pars, char const *str, int border[2]);
	void _h_charset(Parsered_unit &Pars, char const *str, int border[2]);
	void _header_filling(std::multimap<int, std::string>&,\
													char const *, int[2]);
	void _pair_filling(std::pair<std::string,\
										std::string>&, char const *, int[2]);
	void _date_parsing(Date_unit&, char const*, int[2]);
	void _referer_parsing(Parsered_unit &Pars, char const * str, int border[2]);
	void _user_agent_parsing(User_agent&, char const *, int[2]);
	void _www_auth(Auth_unit&, char const *, int[2]);
	void _host_fill(Host_unit&, char const *, int[2]);
	public:
		Parser_unit(void);
		Parser_unit(Parser_unit const & for_copy);
		Parser_unit& operator=(Parser_unit const & for_copy);
		~Parser_unit(void);
		void parse(Request_unit &Request);
		bool getStatus(void);
	
};

#endif
