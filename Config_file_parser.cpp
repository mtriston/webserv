#include "Config_file_parser.hpp"
///ВНИМАНИЕ в файле есть закоменченные области, они убранны для облегчения тестирования (можно найти по комментариям "//to do")
listen_unit & listen_unit::operator=(listen_unit const & for_copy)
{
	str = for_copy.str;
	for (int i = 0; i < 4; i++)
		digit[i] = for_copy.digit[i];
	type = for_copy.type;
	port = for_copy.port;
	return (*this);
}

config_unit& config_unit::operator=(config_unit const & for_copy)
{
	name = for_copy.name;
	location = for_copy.location;
	listen = for_copy.listen;
	methods = for_copy.methods;
	error = for_copy.error;
	cgi_loc = for_copy.cgi_loc;
	max_client_body = for_copy.max_client_body;
	err_location = for_copy.err_location;
	autoindex = for_copy.autoindex;
	return (*this);
}

void Config_parser::_read_file(int fd, int len)
{
	_file.resize(len + 1);
	read(fd, &_file[0], len);	
}

bool Config_parser::_open_file(char const *file_addr)
{
	int fd;
	int len;
	int ext;
	
	ext = strlen(file_addr);
	if (ext < 6 || file_addr[ext - 1] != 'f' || 
		file_addr[ext - 2] != 'n' || file_addr[ext - 3] != 'o' ||
		file_addr[ext - 4] != 'c' || file_addr[ext - 5] != '.')
	{
		write(2, "File extention must be .conf\n", 26);
		return false ;
	}
	fd = open(file_addr, O_RDONLY);
	if (fd < 0)
	{
		write(2, file_addr, strlen(file_addr));
		write(2, " can't be opened\n", 18);
		return false;
	}
	len = lseek(fd, 0, SEEK_END);
	lseek(fd, 0, SEEK_SET);
	_read_file(fd, len);
	close(fd);
	return true;
}

void Config_parser::_pars_location(char const *str)
{
	std::pair<std::string, \
				std::string> 	temp;
	char const*					context;
	int 						cnt;
	
	context = _context(str);
	if (!_normal(context, "server"))
	{
		_act->error = BAD_LOCATION;
		write(2, "Location directive must be inside server\n", 42);
		return ;
	}
	cnt = 8; //lenght of "location"
	while (str[cnt] < 33 && str[cnt] != '\0')
		cnt++;
	if (str[cnt] != '{')
	{
		context = &str[cnt];
		while (str[cnt] > 32 && str[cnt] != '{')
			cnt++;
		temp.first.assign(context, &str[cnt]);
		while (str[cnt] != '{' && str[cnt] != '\0')
		{
			if (str[cnt] > 32)
			{
				while (str[cnt] != '{' && str[cnt] != '\0')
					++cnt;
				write(2, "Location to many arguments: ", 28);
				write(2, context, &str[cnt] - context);
				write(2, "\n", 1);
				_act->error = BAD_LOCATION;
				return ;
			}
			++cnt;
		}
	}
	if (str[cnt++] != '{')
	{
		write(2, "Location to many arguments: ", 28);
		write(2, context, &str[cnt] - context);
		write(2, "\n", 1);
		_act->error = BAD_LOCATION;
		return ;
	}
	while (str[cnt] < 33 && str[cnt] != '\0')
		++cnt;
	context = &str[cnt];
	while (str[cnt] != '}' && str[cnt] > 32)
		++cnt;
	temp.second.assign(context, &str[cnt]);
	while (str[cnt] != '}' && str[cnt] < 33 && str[cnt] != '\0')
	{
		if (str[cnt] > 32)
		{
			while (str[cnt] != '{' && str[cnt] != '\0')
				++cnt;
			write(2, "Location to many arguments: ", 28);
			write(2, context, &str[cnt] - context);
			write(2, "\n", 1);
			_act->error = BAD_LOCATION;
			return ;
		}
		++cnt;
	}
	if (!(_act->location.insert(temp).second))
	{
		write(2, "Location: ", 10);
		write(2, temp.first.c_str(), temp.first.size());
		write(2, " already added\n", 16);
		_act->error = BAD_LOCATION;
	}
}

void Config_parser::_pars_cgi_loc(char const *str)
{
	char const*	context;
	int 		cnt;

	context = _context(str);
	cnt = 0;
	if (!_normal(context, "server"))
	{
		write(2, "cgi_loc bad placed\n", 25);
		_act->error = BAD_CGI_LOC;
		return ;
	}
	while (str[cnt] > 32 && str[cnt] != '{')
		++cnt;
	while (str[cnt] < 33 && str[cnt] != '\0')
		++cnt;
	if (str[cnt] != '{')
	{
		write(2, "cgi_loc must containes {}\n", 32);
		_act->error = BAD_CGI_LOC;
		return ;
	}
	else
		++cnt;
	while (str[cnt] < 33 && str[cnt] != '\0')
		++cnt;
	context = &str[cnt];
	while (str[cnt] != '}' && str[cnt] > 32)
		++cnt;
	_act->cgi_loc.assign(context, &str[cnt]);
	while (str[cnt] != '}' && str[cnt] < 33 && str[cnt] != '\0')
		++cnt;
	if (str[cnt] != '}')
	{	
		while (str[cnt] != '}' && str[cnt] > 32)
			++cnt;
		write(2, "cgi_loc to many arguments: \n", 32);
		write(2, context, &str[cnt] - context);
		write(2, "\n", 1);
		_act->error = BAD_CGI_LOC;
		return ;
	}
}

void Config_parser::_breckets(int pos)
{
	char const*	title;
	int			cnt;

	++pos;
	cnt = 0;
	title = _context(pos);
	if (!_breck)
	{
		if (_normal(title, "server"))
		{
			_conf.push_back(config_unit());
			_act = &_conf.back();
			_act->autoindex = false;
			return ;
		}
		else
		{
			while (title[cnt] != '{')
				++cnt;
			write(2, "Out of server: ", 15);
			write(2, title, cnt);
			write(2, "\n", 1);
			if (_act)
				_act->error = OUT_OF_SER;
			return ;
		}
	}
	if (_normal(title, "location"))
		_pars_location(title);
	else if (_normal(title, "cgi_loc"))
		_pars_cgi_loc(title);
	else if (_normal(title, "error_pages"))
		_pars_error_pages(title);
	else if (_normal(title, "server"))
	{
		_act->error = UNKNWN_TTL;
		write(2, "Server directive must be outside other directive\n", 50);
		return ;
	}
	else
	{
		_act->error = UNKNWN_TTL;
		while (title[cnt] > 32 && title[cnt] != '{')
			++cnt;
		write(2, "Unknown title: ", 15);
		write(2, title, cnt);
		write(2, "\n", 1);
		return ;
	}
}

int Config_parser::_step_back(int cnt)
{
	char const *str;

	str = &_file[0];
	--cnt;
	while (cnt > 0 && str[cnt] != '}' && str[cnt] != ';' && str[cnt] != '{')
		--cnt;
	++cnt;
	while (str[cnt] < 33)
		++cnt;
	return (cnt);
}

void Config_parser::_pars_listen(char const * str)
{
	int			cnt;
	int			fsym;
	listen_unit	temp;
	int			dots;
	int			check;

	cnt = 7;
	temp.type = 'd';
	temp.port = 80;
	dots = 0;
	check = 0;
	fsym = 0;
	for (int i = 0; i < 4; i++)
		temp.digit[i] = 0;
	if (!_normal(_context(str), "server"))
	{
		_act->error = 1;
		write(2, "Listen directive must be inside server\n", 40);
		return ;
	}
	while (str[cnt] < 33 && str[cnt] != '\0')
		++cnt;
	fsym = cnt;
	while (str[cnt] != ':' && str[cnt] != ';' && str[cnt] > 32)
	{
		if (str[cnt] != '.' && (str[cnt] > 57 || str[cnt] < 48))
			temp.type = 's';
		else if (str[cnt] == '.')
			++dots;
		++cnt;
	}
	temp.str.assign(&str[fsym], &str[cnt]);
	if (temp.type == 'd' && dots == 3 && str[fsym] != '\0')
	{
		for (int i = 0; i < 4; i++)
		{
			check = atoi(&str[fsym]);
			if (check < 0 || check > 255)
				temp.type = 's';
			temp.digit[i] = check;
			while (str[fsym] != '\0' && str[fsym] != '.')
				++fsym;
			++fsym;
		}
	}
	if (temp.type == 'd' && !dots)
	{
		temp.port = atoi(&str[fsym]);
		temp.type = 'p';
	}
	if (str[cnt] == ':')
		temp.port = atoi(&str[(++cnt)++]);
	if (temp.port == 0)
		temp.port = 80;
	_act->listen.push_back(temp);
	if (str[cnt] != ';')
		++cnt;
	while (str[cnt] > '0' && str[cnt] < '9')
		++cnt;
	if (str[cnt] != ';')
		while (str[cnt] < 33 && str[cnt] != '\0')
			cnt++;
	if (str[cnt] != ';')
	{
		write(2, "Bad directive: ", 14);
		write(2, str, cnt);
		write(2, "\n", 1);
		_act->error = BAD_LISTEN;
		return ;
	}
}

void Config_parser::_client_body_size(char const *str)
{
	int 		cnt;
	long int 	temp;
	bool		done;
	
	cnt = -1;
	done = false;
	while (str[++cnt] > 32);
	while (str[++cnt] < 33 && str[cnt] != '\0');
	temp = atoi(&str[cnt]);
	while (isdigit(str[cnt]))
		++cnt;
	if (str[cnt] != 'b')
		temp *= 1024;
	else
		++cnt;
	while (str[cnt] < 33 && str[cnt] != ';')
		++cnt;
	if (str[cnt] != ';' || temp < 0)
	{
		_act->error = CL_BODY_SZ;
		write(2, "client_max_body_size must be positive integer\n", 47);
		return ;
	}
	_act->max_client_body = temp;
}


void Config_parser::_autoindex(char const *str)
{
	int cnt;
	
	cnt = 9; //lenght of "autoindex"
	while (str[cnt] < 33 && str[cnt] != '\0')
		++cnt;
	if (!strncmp(&str[cnt], "on", 2))
	{
		_act->autoindex = true;
		cnt += 2;
	}
	else if (!strncmp(&str[cnt], "off", 3))
	{
		_act->autoindex = false;
		cnt += 3;
	}
	else
	{
		write(2, "autoindex must be on/off only\n", 31);
		_act->error = AUTOINDEX;
		return ; 
	}
	while (str[cnt] < 33 && str[cnt] != '\0')
		++cnt;
	if (str[cnt] != ';')
	{
		write(2, "autoindex too many arguments: \n", 30);
		write(2, str, cnt);
		write(2, "\n", 1);	
		_act->error = AUTOINDEX;
	}
}

void Config_parser::_semicolon(int pos)
{
	int cnt;
	char const *str;
	
	cnt = _step_back(pos);
	str = &_file[cnt];
	if (!_breck)
	{
		write(2, "Out of server: ", 15);
		write(2, str, pos - cnt);
		write(2, "\n", 1);
		if (_act)
			_act->error = OUT_OF_SER;
		return ;
	}
	if (_normal(str, "listen"))
		_pars_listen(str);
	else if (_normal(str, "server_name"))
		_name_filling(str);
	else if (_normal(str, "accepted_methods"))
		_methods_filling(str);
	else if (_normal(str, "client_max_body_size"))
		_client_body_size(str);
	else if (_normal(str, "autoindex"))
		_autoindex(str);
	else
	{
		cnt = 0;
		_act->error = UNKNWN_TTL;
		while (str[cnt] > 32 && str[cnt] != ';')
			++cnt;
		write(2, "Unknown string: ", 16);
		write(2, str, cnt);
		write(2, "\n", 1);
		return ;
	}
}

bool Config_parser::_recheck_breckts(char const *str)
{
	int		cnt;
	bool	work;
	
	cnt = -1;
	work = false;
	while (str[++cnt] != '\0')
	{
		if (str[cnt] == '{')
		{
			work = true;
			_breckets(cnt);
			++_breck;
		}
		else if (str[cnt] == '}')
			--_breck;
		else if (str[cnt] == ';')
			_semicolon(cnt);
		if (_act && _act->error)
			break;
	}
	if (_act->error)
		return false;
	else if (cnt < 1)
	{
		_act->error = WRONG_BRCKT;
		write(2, "Empty file\n", 19);
		return false;
	}
	else if (_breck)
	{
		if (_breck > 0)
			write(2, "Not enaught }\n", 15);
		else
			write(2, "Too many }\n", 12);
		return false;
	} 
	else if (!work)
	{
		write(2, "Wrong file\n", 12);
		return false;
	}
	return true;
}

bool Config_parser::_normal(char const *str, char const *orig)
{
	int len;
	
	len = strlen(orig);
	if (!strncmp(str, orig, len))
	{
		if (str[len] < 33 || str[len] == '{')
			return true;
	}
	return false;
}

char const *Config_parser::_context(char const *str)
{
	return (_context(str - &_file[0]));
}

char const *Config_parser::_context(int pos)
{
	char 		res;
	char const *str;
	int 		cnt;
	int			breck;

	cnt = pos;
	str = &_file[0];
	res = -1;
	breck = 0;
	while (cnt > 0)
	{ 	
		if (str[cnt] == '{' && breck == 0)
			break;
		else if (str[cnt] == '{')
			--breck;
		if (str[cnt] == '}')
			++breck;
		--cnt;
	}
	breck = 0;
	while (cnt > 0 && str[cnt] != ';')
	{
		--cnt;
		if (str[cnt] == '{' || str[cnt] == '}')
			break ;
	}
	++cnt;
	while (str[cnt] < 33 && str[cnt] != '\0')
		++cnt;
	return (&str[cnt]);
}

void Config_parser::_name_filling(char const *str)
{
	int	temp;
	int	cnt;
	
	cnt = 0;
	while (str[cnt] > 32)
		++cnt;
	while (str[cnt] != ';' && str[cnt] != '\0')
	{
		while (str[cnt] < 33 && str[cnt] != '\0')
			++cnt;
		temp = cnt;
		while (str[cnt] > 32 && str[cnt] != ';')
			++cnt;
		_act->name.push_back(std::string(&str[temp], &str[cnt]));
		while (str[cnt] < 33 && str[cnt] != '\0')
			++cnt;
	}
}

void Config_parser::_methods_filling(char const *str)
{
	int		temp;
	int		cnt;
	
	cnt = 0;
	while (str[cnt] > 32)
		++cnt;
	while (str[cnt] != ';' && str[cnt] != '\0')
	{
		while (str[cnt] < 33 && str[cnt] != '\0')
			++cnt;
		temp = cnt;
		while (str[cnt] > 32 && str[cnt] != ';')
			++cnt;
		_act->methods.push_back(std::string(&str[temp], &str[cnt]));
		if (_act->methods.back() != "GET" &&
				_act->methods.back() != "POST" &&
					_act->methods.back() != "HEAD" &&
						_act->methods.back() != "OPTIONS")
		{
			write(2, "Unknown HTTP method: ", 21);
			write(2, _act->methods.back().c_str(),
				_act->methods.back().size());
			write(2, "\n", 1);
			return ;
		}
		while (str[cnt] < 33 && str[cnt] != '\0')
			++cnt;
	}
}

void	Config_parser::_pars_error_pages(char const *str)
{
	std::string	temp_path;
	int 		cnt;
	int			temp;
	bool 		ins;
	size_t 		pos;
	int 		err_n;

	cnt = 11; //lenght of "error_pages"
	ins = false;
	if (!_normal(_context(str), "server"))
	{
		write(2, "error_pages directive must be inside server\n", 45);
		_act->error = ERR_PAGES;
		return ;
	}
	while (str[cnt] != '{' && str[cnt] != '\0')
	{
		if ((str[cnt] < 48 || str[cnt] > '9') && str[cnt] > 32)
		{
			write(2, "error_pages directive must containes intergers\n", 48);
			_act->error = ERR_PAGES;
			return ;
		}
		++cnt;
	}
	if (str[cnt] == '{')
		temp = ++cnt;
	else
	{
		write(2, "error_pages directive must containes intergers\n", 48);
		_act->error = ERR_PAGES;
		return ;
	}
	while (str[cnt] != '}' && str[cnt] != '\0')
		++cnt;
	temp_path.assign(&str[temp], &str[cnt]);
	pos = temp_path.find_last_of("^");
	cnt = 11; //lenght of "error_pages"
	temp = 0;
	while (str[cnt] != '{' && str[cnt] != '\0')
	{
		if (str[cnt] > 32 && ins == false)
		{
			err_n = atoi(&str[cnt]);
			ins = true;
			temp = cnt;
			_act->err_location[err_n] = temp_path;
		}
		else if (str[cnt] < 33 && ins == true)
		{	
			if (pos != std::string::npos)
				_act->err_location[err_n].replace(pos, 1, &str[temp], cnt - temp);
			ins = false;
		}
		++cnt;
	}
	if (_act->err_location.empty())
		_act->err_location[0] = temp_path;
}

bool Config_parser::_check_file(std::string const&file)
{
	struct stat stats;
	int 		res;
	
	if (!_check_path(file))
		return false;
	return true;//to do убери эту заглушку, она проверяет фаил по пути
/*	if (stat(file.c_str(), &stats))
	{
		write(2, "Can't accept file: ", 19);
		write(2, file.c_str(), file.size());
		write(2, "\n", 1);
		return false;
	}
	return (true);*/
}

bool Config_parser::_check_dir(std::string const&dir)
{
	struct stat stats;
	int 		res;
	
	if (!_check_path(dir))
		return false;
	return true; //to do убери эту заглушку, она проверяет папку по пути
/*	if (stat(dir.c_str(), &stats))
	{
		write(2, "Can't accept folder: ", 19);
		write(2, dir.c_str(), dir.size());
		write(2, "\n", 1);
		return false;
	}
	if (!S_ISDIR(stats.st_mode))
	{
		write(2, dir.c_str(), dir.size());
		write(2, " is not folder\n", 15);
		return false;
	}
	return true;*/
}

bool Config_parser::_check_path(std::string const& path)
{
	int 		cnt;
	char const 	*str;
	int 		up;
	
	up = 0;
	cnt = _main_folder.size();
	str = path.c_str();
	while (str[cnt] != '\0' && up > -1)
	{
		if (str[cnt] == '.' && str[cnt + 1] == '.' &&\
						str[cnt - 1] == '/')
			--up;
		if (str[cnt] == '/')
			++up;
		if (cnt > 3 && str[cnt] == '/' && str[cnt - 1] == '.'\
					 && str[cnt - 2] == '.' && str[cnt - 3] == '/')
			--up;
		++cnt;
	}
	if (up < 0)
	{
		write(2, path.c_str(), path.size());
		write(2 , " is illegal path\n", 18);
		return false;
	}
	return true;
}

bool Config_parser::_check_location(config_unit &pars)
{
	bool 											ok;
	std::map<std::string, std::string>::iterator	it;
	std::map<std::string, std::string>::iterator	end;	
	
	ok = true;
	end = pars.location.end();
	it = pars.location.begin();
	if (!it->first.empty())
	{
		
		write(2, "Server ", 7);
		write(2, pars.listen.begin()->str.c_str(), pars.listen.begin()->str.size());
		write(2, " doesn't have default location\n", 32);
		return (false);
	}
	while (it != end && ok)
	{
		if (it != pars.location.begin() && it->second[0] == '.')
			it->second.replace(0, 2, pars.location.begin()->second.c_str(),\
										pars.location.begin()->second.size());
		else if (it->second[0] == '/')
			it->second.replace(0, 1, _main_folder.c_str(), _main_folder.size());
		else
			it->second = _main_folder + it->second;
		ok = _check_dir(it->second);
		++it;
	}
	return (ok);
}

void Config_parser::_check_methods(config_unit &pars)
{
	if (_act->methods.empty())
	{
		_act->methods.push_back("GET");
		_act->methods.push_back("POST");
		_act->methods.push_back("HEAD");
		_act->methods.push_back("OPTIONS");
	}
}

bool Config_parser::_check_cgi_loc(config_unit &pars)
{
	if (pars.cgi_loc.empty())
	{
		pars.cgi_loc = pars.location.begin()->second;
		return true;
	}
	if (pars.cgi_loc[0] == '.')
		pars.cgi_loc.replace(0, 1, pars.location.begin()->second.c_str(),\
										pars.location.begin()->second.size());
	else if (pars.cgi_loc[0] == '/')
		pars.cgi_loc.replace(0, 1, _main_folder.c_str(), \
														_main_folder.size());
	else
		pars.cgi_loc.insert(0, _main_folder);
	return (_check_dir(pars.cgi_loc));
}



bool Config_parser::_check_err_loc(config_unit &pars)
{
	bool 											ok;
	std::map<int, std::string>::iterator	it;
	std::map<int, std::string>::iterator	end;	
	
	ok = true;
	end = pars.err_location.end();
	it = pars.err_location.begin();
	while (it != end && ok)
	{
		if (it->second[0] == '.')
			it->second.replace(0, 2, pars.location.begin()->second.c_str(),\
										pars.location.begin()->second.size());
		else if (it->second[0] == '/')
			it->second.replace(0, 1, _main_folder.c_str(), _main_folder.size());
		else
			it->second = _main_folder + it->second;
		ok = _check_dir(it->second);
		++it;
	}
	return (ok);
}


bool Config_parser::_check_parsed_data(void)
{
	std::list<config_unit>::iterator 	it;
	std::list<config_unit>::iterator 	end;
	bool 								ok;
	
	it = _conf.begin();
	end = _conf.end();
	ok = true;
	while (it != end && ok)
	{
		ok = _check_location(*it);
		_check_methods(*it);
		_check_cgi_loc(*it);
		_check_err_loc(*it);
		if (it->max_client_body == 0)
			it->max_client_body = -1;
		++it;
	}
	return (ok);
}

bool Config_parser::init(char const * file_addr)
{	
	_breck = 0;
	_act = NULL;
	if (!_open_file(file_addr))
		return false;
	if (!_recheck_breckts(&_file[0]))
		return false;
	if (!_conf.empty())
	{
		if (!_check_parsed_data())
			_act->error = 100;
		_map_filling();
	}
	_file.clear();
	if (_act && !_act->error)
		return true;
	return false;
}

void Config_parser::_map_filling(void)
{
	std::list<config_unit>::iterator conf_b;
	std::list<config_unit>::iterator conf_e;
	std::list<listen_unit>::iterator li_it_b;
	std::list<listen_unit>::iterator li_it_e;
	
	conf_b = _conf.begin();
	conf_e = _conf.end();
	while (conf_b != conf_e)
	{
		
		li_it_b = conf_b->listen.begin();
		li_it_e = conf_b->listen.end();
		while (li_it_b != li_it_e)
		{
			_ports[li_it_b->port].push_back(&*conf_b);
			++li_it_b;
		} 
		++conf_b;
	}
}

Config_parser::Config_parser(void){
	_main_folder = "/tmp/ft_www/";
}
Config_parser::~Config_parser(void){}
Config_parser::Config_parser(Config_parser& for_copy)
{
	*this = for_copy;
}
Config_parser& Config_parser::operator=(Config_parser const & for_copy)
{
	_file = for_copy._file;
	return (*this);
}

std::list<config_unit> &Config_parser::getConf(void)
{
	return (_conf);
}

std::map<int, std::list<config_unit*> > const& Config_parser::getPortsMap(void)
{
	return (_ports);
}