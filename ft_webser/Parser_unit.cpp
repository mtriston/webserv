#include "Parser_unit.hpp"

Parser_unit::Parser_unit(void){_work = true;}
Parser_unit::Parser_unit(Parser_unit const & for_copy){_work = true;}
Parser_unit& Parser_unit::operator=(Parser_unit const & for_copy){return *this;}
Parser_unit::~Parser_unit(void){};



bool Parser_unit::_str_step(char const *str, \
						int &cnt, int border, char ch, bool neg)
{
//	std::cout << "Char |" << str[cnt] << "|!!!" << cnt << "\n";
	if (neg == false)
	{
//		std::cout << "neg\n";
		while (str[cnt] != '\0' && str[cnt] != ch && cnt < border)
			cnt++;
	}
	else
	{
		while (str[cnt] != '\0' && str[cnt] == ch && cnt < border)
			cnt++;
	}
//	std::cout << "Char |" << str[cnt] << "|!!!" << cnt << "\n";
	if (cnt == border)
		return true;
	return false;
}


void Parser_unit::_crlf_spliting(int *borders, char const *str)
{
	if (str[borders[1]] == 13)
		borders[0] = borders[1] + 2;
	borders[1] = borders[0];
	
	while (str[borders[1]] != '\0')
	{
		if (str[borders[1]] == 13 && str[borders[1] + 1] == 10)
			return ;
		++borders[1];
	}
}

void Parser_unit::_first_line(int border, char const*str, Parsered_unit &Pars)
{
	int fs;
	int ls;
	
	Pars.error = 0; //to do перенести отсюда
	fs = -1;
//	while (str[++fs] == 32);
//std::cout << "first line\n";
	if (_str_step(str, fs, border, ' ', true))
		return (void)(Pars.error = 1);
	if (fs > border)
	{
		Pars.error = 1;
		return ;
	}
//std::cout << "1\n";
	if (!strncmp(&str[fs], "GET ", 4))
	{
		fs += 4;
		Pars.method = GET;
	}
	else if (!strncmp(&str[fs], "PUT ", 4))
	{
		fs += 4;
		Pars.method = PUT;
	}
	else if (!strncmp(&str[fs], "POST ", 5))
	{
		fs += 5;
		Pars.method = POST;
	}
	else if (!strncmp(&str[fs], "HEAD ", 5))
	{
		fs += 5;
		Pars.method = HEAD;
	}
	else
		return (void)(Pars.error = 1);
//std::cout << "2\n";
	while (str[++fs] == 32);
	ls = border;
//std::cout << "3\n";
	while (str[ls - 1] != '.')
		--ls;
//std::cout << "4\n";
	Pars.prot_ver[1] = atoi(&str[ls]);
//std::cout << "5\n";
	while (str[ls - 1] != '/')
		--ls;
//std::cout << "\n";
	Pars.prot_ver[0] = atoi(&str[ls]);
	while (str[ls] != 'H')
		--ls;
	while (str[ls] == 32)
		--ls;
	Pars.path.assign(&str[--fs], &str[ls]);
//std::cout << "first line out\n";
}




void Parser_unit::_char_split(char const *str, int *borders, char ch, int stop)
{
	if (str[borders[1]] == ch)
		borders[0] = borders[1] + 1;
	borders[1] = borders[0];
	
	while (borders[1] < stop)
	{
		if (str[borders[1]] == ch)
			return ;
		++borders[1];
	}
}

void Parser_unit::_header_filling(std::multimap<int, std::string> &mmap, \
	char const *str, int border[2])
{
	int i = border[0];
	int act_position;
	int inner_borders[2];
	std::pair<int, std::string> adding_elem;
	bool parsed = false;
	
	while (str[i] != ':')
		++i;
	++i;
	while (str[i] == 32)
		++i;
	inner_borders[0] = i;
	inner_borders[1] = i;
	while (i < border[1])
	{
		if (str[i] == ';' && str[i+1] == 'q' && str[i+2] == '=')
		{
			parsed = true;
			act_position = i + 3;
			while (str[act_position] != '.')
				++act_position;
			if (str[act_position++ - 1] > '0')  //to do проверь поведение при "больше 1"
				adding_elem.first = 1;
			else
				adding_elem.first = atoi(&str[act_position]);
			while (inner_borders[1] < i - 1)
			{
				_char_split(str, inner_borders, ',', i);
				adding_elem.second.assign(&str[inner_borders[0]],\
												&str[inner_borders[1]]);
				mmap.insert(adding_elem);
			}
		}
		++i;
	}
	if (!parsed)
	{
		while (inner_borders[1] < i - 1)
		{
			_char_split(str, inner_borders, ',', i);
			adding_elem.second.assign(&str[inner_borders[0]],\
										&str[inner_borders[1]]);
			mmap.insert(adding_elem);
		}
	}
}

void Parser_unit::_pair_filling(std::pair<std::string,std::string> &pair,\
						 char const *str, int border[2])
{
	int inner_borders[2];
	int i = border[0];
	
	while (str[i] != ':')
		++i;
	++i;
	while (str[i] == 32)
		++i;
	inner_borders[0] = i;
	inner_borders[1] = i;
	_char_split(str, inner_borders, ' ', border[1]);
	pair.first.assign(&str[inner_borders[0]], &str[inner_borders[1]]);
	pair.second.assign(&str[inner_borders[1]], &str[border[1] - 1]);
}

void Parser_unit::_referer_parsing(Parsered_unit &Pars, \
							char const * str, int border[2])
{
	int i;
	
	i = border[0];
	if (_str_step(str, i, border[1], ' ', false) ||
		_str_step(str, i, border[1], ' ', false))
		{
			std::cout << "REFERER PARSING WRONG\n";
			Pars.error = 1;
			return ;
		}
	Pars.referer_str.assign(&str[i], &str[border[1]]);
}

void Parser_unit::_date_parsing(Date_unit& _date, char const *str, \
														int boders[2])
{
	int i;

	i = boders[0];
	_date.error = 0;
	if (_str_step(str, i, boders[1], ':', false))
		return (void)(_date.error = 1);
	++i;
	if (_str_step(str, i, boders[1], ' ', true))
		return (void)(_date.error = 1);
	_date.day_name.assign(&str[i], &str[i + 3]); 
		//to do проверь запятую после дня недели
	if (_date.day_name == "Mon")
		_date.day_name_int = 1;
	else if (_date.day_name == "Tue")
		_date.day_name_int = 2;
	else if (_date.day_name == "Wed")
		_date.day_name_int = 3;
	else if (_date.day_name == "Thu")
		_date.day_name_int = 4;
	else if (_date.day_name == "Fri")
		_date.day_name_int = 5;
	else if (_date.day_name == "Sat")
		_date.day_name_int = 6;
	else if (_date.day_name == "Sun")
		_date.day_name_int = 7;
	else
		_date.error = 1;
	i += 3;
	if (_str_step(str, ++i, boders[1], ' ', true))
		return (void)(_date.error = 1);
	_date.day = atoi(&str[i]);
	if (_str_step(str, i, boders[1], ' ', true))
		return (void)(_date.error = 1);
	_date.mounth_name.assign(&str[i], &str[i + 3]);
	if (_date.mounth_name == "Jan")
		_date.mounth_int = 1;
	else if (_date.mounth_name == "Feb")
		_date.mounth_int = 2;
	else if (_date.mounth_name == "Mar")
		_date.mounth_int = 3;
	else if (_date.mounth_name == "Apr")
		_date.mounth_int = 4;
	else if (_date.mounth_name == "May")
		_date.mounth_int = 5;
	else if (_date.mounth_name == "Jun")
		_date.mounth_int = 6;
	else if (_date.mounth_name == "Jul")
		_date.mounth_int = 7;
	else if (_date.mounth_name == "Aug")
		_date.mounth_int = 8;
	else if (_date.mounth_name == "Sep")
		_date.mounth_int = 9;
	else if (_date.mounth_name == "Oct")
		_date.mounth_int = 10;
	else if (_date.mounth_name == "Nov")
		_date.mounth_int = 11;
	else if (_date.mounth_name == "Dec")
		_date.mounth_int = 12;
	else
		_date.error = 1;
	if (_date.day < 1)
		return (void)(_date.error = 1);
	if (_date.mounth_int == 1 ||_date.mounth_int == 3 || \
		_date.mounth_int == 5 || _date.mounth_int == 7 || \
		_date.mounth_int == 8 || _date.mounth_int == 10 || \
		_date.mounth_int ==12)
	{
		if (_date.day > 31)
			return (void)(_date.error = 1);
			
	}
	else
	{
		if (_date.day > 30)
			return (void)(_date.error = 1);
	}
	if (_date.mounth_int == 2 && _date.day > 29)
		return (void)(_date.error = 1);
	i += 3;
	if (_str_step(str, i, boders[1], ' ', true))
		return (void)(_date.error = 1);

	_date.hour = atoi(&str[i]);
	if (_date.hour < 0 || _date.hour > 23)
		return (void)(_date.error = 1);
	i += 2;
	if (str[i++] != ':')//to do проверь точно ли
		return (void)(_date.error = 1);

	_date.minute = atoi(&str[i]);
	if (_date.minute < 0 || _date.minute > 59)
		return (void)(_date.error = 1);
	i += 2;
	if (str[i++] != ':')//to do проверь точно ли
		return (void)(_date.error = 1);

	_date.second = atoi(&str[i]);
	if (_date.second < 0 || _date.second > 59)
		return (void)(_date.error = 1);
	i += 2;
	if (str[i++] != ':')//to do проверь точно ли
		return (void)(_date.error = 1);
	
	if (_str_step(str, i, boders[1], ' ', true))
		return (void)(_date.error = 1);
	if (str[i] != 'G' || str[i + 1] != 'T'|| str[i + 2] != 'M')
		return (void)(_date.error = 1);
	
	while (str[i] == 32)
		i++;
	if (str[i] != 13)
		return (void)(_date.error = 1);
	_date.empty = 0;
}

void Parser_unit::_user_agent_parsing(User_agent &usr,\
								char const *str, int border[2])
{
	int i;
	int inner_border;
	
	i = border[0];
	usr.empty = 0;
	if (_str_step(str, i, border[1], ':', false))
		return (void)(usr.error = 1);
	if (_str_step(str, i, border[1], ' ', true))
		return (void)(usr.error = 1);
	inner_border = i;
	if (_str_step(str, i, border[1], '/', false))
		return (void)(usr.error = 1);
	usr.product_str.assign(&str[inner_border], &str[i - 1]);
	usr.product_ver[0] = atoi(&str[i + 1]);
	if (_str_step(str, i, border[1], '.', false))
		return (void)(usr.error = 1);
	usr.product_ver[0] = atoi(&str[i + 1]);
	if (_str_step(str, i, border[1], ' ', false))
		return (void)(usr.error = 1);
	if (_str_step(str, i, border[1], ' ', true))
		return (void)(usr.error = 1);
	usr.comment_str.assign(&str[i], &str[border[1]]);
	usr.error = 0;
	if (str[i] == '(')
	{
		inner_border = i;
		if (_str_step(str, i, border[1], ')', false))
			return ;
		usr.information_str.assign(&str[inner_border], &str[i++]);
		if (_str_step(str, i, border[1], ' ', true))
			return ;
		inner_border = i;
		if (_str_step(str, i, border[1], ' ', false))
			return ;
		usr.platform_str.assign(&str[inner_border], &str[i++]);
		if (_str_step(str, i, border[1], ' ', true))
			return ;
		inner_border = i;
		if (_str_step(str, i, border[1], ' ', false))
			return ;
		usr.details_str.assign(&str[inner_border], &str[i++]);
		if (_str_step(str, i, border[1], ' ', true))
			return ;
		usr.extensions.assign(&str[i], &str[border[1]]);
	}
	
}

void Parser_unit::_www_auth(Auth_unit& auth, char const *str, int border[2])
{
	int i;
	char const *realm;
	char const *charset;
	int inner_borders[0];
	
	i = border[0];
	inner_borders[0] = 0;
	if (_str_step(str, i, border[1], ' ', false))
		return (void)(auth.error = 1);
	if (_str_step(str, i, border[1], ' ', true))
		return (void)(auth.error = 1);
	if (!(realm = strstr(&str[i], "realm")))
		return (void)(auth.error = 1);
	auth.type.assign(&str[i], realm);
	if (_str_step(realm, inner_borders[0], border[1], '"', false))
		return (void)(auth.error = 1);
	_char_split(realm, inner_borders, '"', border[1]);
	auth.realm.assign(&realm[inner_borders[0]], &realm[inner_borders[1]]);
	if ((charset = strstr(&realm[inner_borders[1]], ", charset=")))
	{
		inner_borders[0] = 0;
		if (_str_step(charset, inner_borders[0], border[1], '"', false))
			return (void)(auth.error = 1);
		_char_split(realm, inner_borders, '"', border[1]);
		auth.charset.assign(&charset[inner_borders[0]], &charset[inner_borders[1]]);
	}
}

void Parser_unit::_host_fill(Host_unit& host, char const *str, int border[2])
{
	int cnt;
	int dot_cnt;
	int temp;
	int check;

	dot_cnt = 0;
	cnt = border[0];
	host.error = false;
	if (_str_step(str, cnt, border[1], ' ', false))
		return (void)(host.error = true);
	if (_str_step(str, cnt, border[1], ' ', true))
		return (void)(host.error = true);
	temp = cnt++;
	while (str[cnt] != '\n' && str[cnt] != ':' && str[cnt] != ' ')
	{
		if ((str[cnt] > 57 || str[cnt] < 48) && str[cnt] != '.')
			break ;
		if (str[cnt] == '.')
			++dot_cnt;
		++cnt;
	}
	_str_step(str, cnt, border[1], ':', false);
	host.name.assign(&str[temp], &str[cnt]);
	if (str[cnt] == ':')
	{
		host.port = atoi(&str[cnt + 1]);
		if (host.port < 0)
			(host.error = true);
	}
	if (dot_cnt == 3 && \
			(str[cnt] == ':' || str[cnt] == ' ' || str[cnt] == '\n'))
	{
		host.type = 'd';
		for (int i = 0; i < 4; i++)
		{
			check = atoi(&str[temp]);
			if (check < 0 || check > 255)
				return (void)(host.error = true);
			host.addr[i] = check;
			while (str[temp] != '.')
				++temp;
			++temp;
		}
	}
	else
		host.type = 's';
}
 
//to do убери заглавные и проедься по всему хедеру, tolow
//разберись какие хедеры только для запросов, кажется Content-Location нет))
void Parser_unit::_h_host(Parsered_unit &Pars, char const *str, int border[2])
{
	if (Pars.host.name.empty() && !strncmp(&str[border[0]], "Host:", 5))
		_host_fill(Pars.host, str, border);
//		Pars.host_str.assign(&str[border[0] + 6], &str[border[1]]);
	else if (Pars.a_charset_mmp.empty() && \
					!strncmp(&str[border[0]], "Accept-Charset:", 15))
		_header_filling(Pars.a_charset_mmp, str, border);
	else if (Pars.a_language_mmp.empty() && \
					!strncmp(&str[border[0]], "Accept-Language:", 16))
		_header_filling(Pars.a_language_mmp, str, border);
	else if (Pars.c_language_mmp.empty() && \
					!strncmp(&str[border[0]], "Content-Language:", 17))
		_header_filling(Pars.c_language_mmp, str, border);
	else if (!strncmp(&str[border[0]], "Content-Length:", 15))
		Pars.c_length_uli = atoi(&str[border[0] + 15]);
	else if (Pars.c_location_mmp.empty() && \
					!strncmp(&str[border[0]], "Content-Location:", 17))
		_header_filling(Pars.c_location_mmp, str, border);
	else if (Pars.c_type_mmp.empty() && \
					!strncmp(&str[border[0]], "Content-Type:", 13))
		_header_filling(Pars.c_type_mmp, str, border);
	else if (Pars.allow_mmp.empty() && \
					!strncmp(&str[border[0]], "Allow:", 6))
		_header_filling(Pars.allow_mmp, str, border);
	else if (!strncmp(&str[border[0]], "Authorization:", 14))
		_pair_filling(Pars.authorization_pr, str, border);
	else if (Pars.date_sc.empty && \
					!strncmp(&str[border[0]], "Date:", 5))
		_date_parsing(Pars.date_sc, str, border);
	else if (Pars.referer_str.empty() &&\
					!strncmp(&str[border[0]], "Referer:", 8))
		_referer_parsing(Pars, str, border);
	else if (Pars.user_agent_s.empty &&\
					!strncmp(&str[border[0]], "User-Agent:", 11))
		_user_agent_parsing(Pars.user_agent_s, str, border);
	else if (!strncmp(&str[border[0]], "WWW-Authenticate:", 17))
		_www_auth(Pars.www_auth, str, border);
}

void Parser_unit::_crlf_split(Request_unit &Request)
{
	int size;
	int borders[2];
	char const * Recieved_string;

	Recieved_string = Request.getReceivedLine().c_str();
	Parsered_unit &Parse = Request.parsed();
	size = Parse.body_pt - Request.getReceivedLine().c_str();
//	size = Request.getReceivedLine().size();
	borders[0] = 0;
	borders[1] = 0;
	_crlf_spliting(borders, Recieved_string);
	_first_line(borders[1], \
			Recieved_string, Request.parsed());
	while (borders[1] < size)
	{
		_crlf_spliting(borders, Recieved_string);
		Parse._splited_rl.push_back(
			std::string(Recieved_string, 
				borders[0], borders[1] - borders[0]));
		if (Parse._splited_rl.back().size() == 0)
			break;
		_h_host(Request.parsed(), Recieved_string, borders);//to do rename
//		_h_charset(Request.parsed(), Recieved_string, borders);
	}
/*	Parse._splited_rl.push_back("body bot");
	Parse._splited_rl.push_back(
			std::string(Request.getReceivedLine().c_str(), 
				borders[0], size - borders[0]));
*/
	Parse.body.assign(Parse.body_pt, &Parse.body_pt[Parse.c_length_uli]);
}


void Parser_unit::parse(Request_unit &Request)
{
	size_t pos;
	_crlf_split(Request);
	Request.Status() = PARSED;
/*	
	pos = Request.getReceivedLine().find("stop");
	if (pos != std::string::npos)
		_work = false;
	pos = Request.getReceivedLine().find("cgi");
	if (pos != std::string::npos)
	{
		Request.getReceivedLine() = &Request.getReceivedLine()[pos + 3];
		Request.Status() = CGI;
		return ;
	}
	pos = Request.getReceivedLine().find("HTTP");
	if (pos != std::string::npos)
	{
		Request.Status() = SAVE_FILE;
		return ;
	}
	else
		Request.Status() = PARSED;*/
}

bool Parser_unit::getStatus(void)
{
	return (_work);
}