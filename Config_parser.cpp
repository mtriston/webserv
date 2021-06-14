#include <algorithm>
#include "Config_parser.hpp"

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
	    file_addr[ext - 4] != 'c' || file_addr[ext - 5] != '.') {
		write(2, "File extention must be .conf\n", 26);
		return false;
	}
	fd = open(file_addr, O_RDONLY);
	if (fd < 0) {
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
                location_unit> temp;
	char const *context;
	int cnt;

	context = _context(str);
	if (!_normal(context, "server")) {
		_error = BAD_LOCATION;
		write(2, "Location directive must be inside server\n", 42);
		return;
	}
	cnt = 8; //lenght of "location"
	while (str[cnt] < 33 && str[cnt] != '\0')
		cnt++;
	if (str[cnt] != '{') {
		context = &str[cnt];
		while (str[cnt] > 32 && str[cnt] != '{')
			cnt++;
		temp.first.assign(context, &str[cnt]);
		if (str[cnt] != '/')
			temp.first.append("/");
		while (str[cnt] != '{' && str[cnt] != '\0') {
			if (str[cnt] > 32) {
				while (str[cnt] != '{' && str[cnt] != '\0')
					++cnt;
				write(2, "Location to many arguments: ", 28);
				write(2, context, &str[cnt] - context);
				write(2, "\n", 1);
				_error = BAD_LOCATION;
				return;
			}
			++cnt;
		}
	}
	if (str[cnt++] != '{') {
		write(2, "Location to many arguments: ", 28);
		write(2, context, &str[cnt] - context);
		write(2, "\n", 1);
		_error = BAD_LOCATION;
		return;
	}
	while (str[cnt] < 33 && str[cnt] != '\0')
		++cnt;
	_a_loc = &temp.second;
	while (str[cnt] != '}' && str[cnt] != '\0') {
		if (str[cnt] == ';')
			_semicolon(&str[cnt] - &_file[0]);
		++cnt;
	}
	if (!(_act->setLocation().insert(temp).second)) {
		write(2, "Location: ", 10);
		write(2, temp.first.c_str(), temp.first.size());
		write(2, " already added\n", 16);
		_error = BAD_LOCATION;
	}
	_a_loc = NULL;
}

void Config_parser::_pars_def_file(char const *str)
{
	char const *context;
	int cnt;
	std::string temp;

	context = _context(str);
	if (!_normal(context, "server") && !_normal(context, "location")) {
		write(2, "default_file bad placed\n", 25);
		_error = BAD_CGI_LOC;
		return;
	}
	cnt = 12;//lenght of "default_file"
	while (str[cnt] < 33 && str[cnt] != '\0' && str[cnt] != ';')
		++cnt;
	context = &str[cnt];
	while (str[cnt] > 32 && str[cnt] != ';')
		++cnt;
	temp.assign(context, &str[cnt]);
	if (_a_loc)
		_a_loc->_def_file = temp;
	else
		_act->setDefaultFile(temp);
	if (str[cnt] != ';') {
		while (str[cnt] < 33 && str[cnt] != '\0' && str[cnt] != ';')
			++cnt;
	}
	if (str[cnt] != ';') {
		_error = 13;
		write(2, "default_file too many arguments\n", 33);
	}
}

void Config_parser::_pars_cgi_loc(char const *str)
{
	char const *context;
	int cnt;

	context = _context(str);
	cnt = 0;
	if (!_normal(context, "server")) {
		write(2, "cgi_loc bad placed\n", 20);
		_error = BAD_CGI_LOC;
		return;
	}
	while (str[cnt] > 32 && str[cnt] != '{')
		++cnt;
	while (str[cnt] < 33 && str[cnt] != '\0')
		++cnt;
	if (str[cnt] != '{') {
		write(2, "cgi_loc must containes {}\n", 32);
		_error = BAD_CGI_LOC;
		return;
	} else
		++cnt;
	while (str[cnt] < 33 && str[cnt] != '\0')
		++cnt;
	context = &str[cnt];
	while (str[cnt] != '}' && str[cnt] > 32)
		++cnt;
	_act->setCGI_loc().assign(context, &str[cnt]);
	while (str[cnt] != '}' && str[cnt] < 33 && str[cnt] != '\0')
		++cnt;
	if (str[cnt] != '}') {
		while (str[cnt] != '}' && str[cnt] > 32)
			++cnt;
		write(2, "cgi_loc to many arguments: \n", 32);
		write(2, context, &str[cnt] - context);
		write(2, "\n", 1);
		_error = BAD_CGI_LOC;
		return;
	}
}

void Config_parser::_breckets(int pos)
{
	char const *title;
	int cnt;

	++pos;
	cnt = 0;
	title = _context(pos);
	if (!_breck) {
		if (_normal(title, "server")) {
			_conf.push_back(config_unit());
			_act = &_conf.back();
			_act->setAutoindex() = 0;
			return;
		} else {
			while (title[cnt] != '{')
				++cnt;
			write(2, "Out of server: ", 15);
			write(2, title, cnt);
			write(2, "\n", 1);
			_error = OUT_OF_SER;
			return;
		}

	}
	if (_normal(title, "location"))
		_pars_location(title);
	else if (_normal(title, "cgi_location"))
		_pars_cgi_loc(title);
	else if (_normal(title, "error_pages"))
		_pars_error_pages(title);
	else if (_normal(title, "server")) {
		_error = UNKNWN_TTL;
		write(2, "Server directive must be outside other directive\n", 50);
		return;
	} else {
		_error = UNKNWN_TTL;
		while (title[cnt] > 32 && title[cnt] != '{')
			++cnt;
		write(2, "Unknown title: ", 15);
		write(2, title, cnt);
		write(2, "\n", 1);
		return;
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

void Config_parser::_pars_listen(char const *str)
{
	int cnt;
	int fsym;
	listen_unit temp;
	int dots;
	int check;

	cnt = 7;
	temp.type = 'd';
	temp.port = 80;
	dots = 0;
	check = 0;
	fsym = 0;
	for (int i = 0; i < 4; i++)
		temp.digit[i] = 0;
	if (!_normal(_context(str), "server")) {
		if (_act)
			_error = 1;
		write(2, "Listen directive must be inside server\n", 40);
		return;
	}
	while (str[cnt] < 33 && str[cnt] != '\0')
		++cnt;
	fsym = cnt;
	while (str[cnt] != ':' && str[cnt] != ';' && str[cnt] > 32) {
		if (str[cnt] != '.' && (str[cnt] > 57 || str[cnt] < 48))
			temp.type = 's';
		else if (str[cnt] == '.')
			++dots;
		++cnt;
	}
	temp.str.assign(&str[fsym], &str[cnt]);
	if (temp.type == 'd' && dots == 3 && str[fsym] != '\0') {
		for (int i = 0; i < 4; i++) {
			check = atoi(&str[fsym]);
			if (check < 0 || check > 255)
				temp.type = 's';
			temp.digit[i] = check;
			while (str[fsym] != '\0' && str[fsym] != '.')
				++fsym;
			++fsym;
		}
	}
	if (temp.type == 'd' && !dots) {
		temp.port = atoi(&str[fsym]);
		temp.type = 'p';
	}
	if (str[cnt] == ':')
		temp.port = atoi(&str[(++cnt)++]);
	if (temp.port == 0)
		temp.port = 80;
	_act->setListen().push_back(temp);
	if (str[cnt] != ';')
		++cnt;
	while (str[cnt] > '0' && str[cnt] < '9')
		++cnt;
	if (str[cnt] != ';')
		while (str[cnt] < 33 && str[cnt] != '\0')
			cnt++;
	if (str[cnt] != ';') {
		write(2, "Bad directive: ", 14);
		write(2, str, cnt);
		write(2, "\n", 1);
		_error = BAD_LISTEN;
		return;
	}
}

void Config_parser::_client_body_size(char const *str)
{
	int cnt;
	size_t temp;
	bool done;

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
	if (str[cnt] != ';' || temp < 0) {
		_error = CL_BODY_SZ;
		write(2, "client_max_body_size must be positive integer\n", 47);
		return;
	}
	if (_a_loc)
		_a_loc->_body_size = temp;
	else
		_act->setMax_client_body() = temp;
}

void Config_parser::_autoindex(char const *str)
{
	int cnt;

	cnt = 9; //lenght of "autoindex"
	while (str[cnt] < 33 && str[cnt] != '\0')
		++cnt;
	if (!strncmp(&str[cnt], "on", 2)) {
		if (_a_loc)
			_a_loc->_autoindex = 1;
		else
			_act->setAutoindex() = 1;
		cnt += 2;
	} else if (!strncmp(&str[cnt], "off", 3)) {
		if (_a_loc)
			_a_loc->_autoindex = 2;
		else
			_act->setAutoindex() = 2;
		cnt += 3;
	} else {
		write(2, "autoindex must be on/off only\n", 31);
		_error = AUTOINDEX;
		return;
	}
	while (str[cnt] < 33 && str[cnt] != '\0')
		++cnt;
	if (str[cnt] != ';') {
		write(2, "autoindex too many arguments: \n", 30);
		write(2, str, cnt);
		write(2, "\n", 1);
		_error = AUTOINDEX;
	}
}

void Config_parser::_pars_redirection(char const *str)
{
	if (!_normal(_context(str), "location")) {
		_error = 1;
		write(2, "Redirection must be location derective\n", 40);
		return;
	}
	if (!_a_loc)
		return;
	int cnt;
	char const *start;
	std::pair<int, std::string> pair;
	cnt = 8; //lenght of redirect
	pair.first = atoi(&str[cnt]);
	if (pair.first < 1) {
		_error = 1;
		write(2, "Redirection must have code\n", 28);
		return;
	}
	while (str[cnt] != ';' && str[cnt] != '\0') {
		if (str[cnt] > 32 && (str[cnt] > 57 || str[cnt] < 48))
			break;
		++cnt;
	}
	start = &str[cnt];
	while (str[cnt] != ';' && str[cnt] != '\0')
		++cnt;
	pair.second.assign(start, &str[cnt]);
	_a_loc->_redirect = pair;
}

void Config_parser::_pars_workers(char const *str)
{
	int cnt;
	int temp;

	cnt = 16; //lenght of "worker_processes"
	temp = atoi(&str[cnt]);
	while (str[cnt] != ';' && str[cnt] != '\0') {
		if (str[cnt] > 32 && (str[cnt] > 57 || str[cnt] < 48))
			break;
		++cnt;
	}
	if (str[cnt] != ';' || temp < 1) {
		write(2, "worker_processes must be positive integer\n", 42);
		_error = BAD_WORKERS;
	}
	_act->setWorkers(temp);
}

void Config_parser::_pars_loc_path(char const *str)
{
	if (_a_loc == NULL)
		return;
	char const *context;
	int cnt;
	std::string temp;
	cnt = 4;//lenght of "path"
	while (str[cnt] < 33 && str[cnt] != '\0' && str[cnt] != ';')
		++cnt;
	context = &str[cnt];
	while (str[cnt] > 32 && str[cnt] != ';')
		++cnt;
	temp.assign(context, &str[cnt]);
	_a_loc->_abs_path = temp;
	if (str[cnt] != ';') {
		while (str[cnt] < 33 && str[cnt] != '\0' && str[cnt] != ';')
			++cnt;
	}
	if (str[cnt] != ';') {
		_error = 13;
		write(2, "path too many arguments\n", 25);
	}
}

bool Config_parser::_semicolon(int pos)
{
	int cnt;
	char const *str;

	cnt = _step_back(pos);
	str = &_file[cnt];
	if (!_breck) {
		write(2, "Out of server: ", 15);
		write(2, str, pos - cnt);
		write(2, "\n", 1);
		if (_act)
			_error = OUT_OF_SER;
		return false;
	}
	if (_normal(str, "listen"))
		_pars_listen(str);
	else if (_normal(str, "server_name"))
		_name_filling(str);
	else if (_normal(str, "accepted_methods"))
		_methods_filling(str);
	else if (_normal(str, "client_max_body_size"))
		_client_body_size(str);
	else if (_normal(str, "max_body_size"))
		_client_body_size(str);
	else if (_normal(str, "autoindex"))
		_autoindex(str);
	else if (_normal(str, "worker_processes"))
		_pars_workers(str);
	else if (_normal(str, "default_file"))
		_pars_def_file(str);
	else if (_normal(str, "path"))
		_pars_loc_path(str);
	else if (_normal(str, "redirect"))
		_pars_redirection(str);
	else if (_normal(str, "file_storage"))
		_pars_storage(str);
	else if (_normal(str, "python_exec"))
		_pars_python_exec(str);
	else if (_normal(str, "php_exec"))
		_pars_php_exec(str);
	else {
		cnt = 0;
		_error = UNKNWN_TTL;
		while (str[cnt] > 32 && str[cnt] != ';')
			++cnt;
		write(2, "Unknown string: ", 16);
		write(2, str, cnt);
		write(2, "\n", 1);
		return false;
	}
	if (_act)
		return true;
	return false;
}

bool Config_parser::_recheck_breckts(char const *str)
{
	int cnt;
	bool work;

	cnt = -1;
	work = false;
	while (str[++cnt] != '\0') {
		if (str[cnt] == '{') {
			work = true;
			_breckets(cnt);
			++_breck;
		} else if (str[cnt] == '}')
			--_breck;
		else if (str[cnt] == ';')
			if (!_semicolon(cnt))
				break;
		if (_error)
			break;
	}
	if (_error)
		return false;
	else if (cnt < 1) {
		_error = WRONG_BRCKT;
		write(2, "Empty file\n", 19);
		return false;
	} else if (_breck) {
		if (_breck > 0)
			write(2, "Not enaught }\n", 15);
		else
			write(2, "Too many }\n", 12);
		return false;
	} else if (!work) {
		write(2, "Wrong file\n", 12);
		return false;
	}
	return true;
}

bool Config_parser::_normal(char const *str, char const *orig)
{
	int len;

	len = strlen(orig);
	if (!strncmp(str, orig, len)) {
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
	char res;
	char const *str;
	int cnt;
	int breck;

	cnt = pos;
	str = &_file[0];
	res = -1;
	breck = 0;
	while (cnt > 0) {
		if (str[cnt] == '{' && breck == 0)
			break;
		else if (str[cnt] == '{')
			--breck;
		if (str[cnt] == '}')
			++breck;
		--cnt;
	}
	breck = 0;
	while (cnt > 0 && str[cnt] != ';') {
		--cnt;
		if (str[cnt] == '{' || str[cnt] == '}')
			break;
	}
	if (cnt > 0)
		++cnt;
	while (str[cnt] < 33 && str[cnt] != '\0')
		++cnt;
	return (&str[cnt]);
}

void Config_parser::_name_filling(char const *str)
{
	int temp;
	int cnt;

	cnt = 0;
	while (str[cnt] > 32)
		++cnt;
	while (str[cnt] != ';' && str[cnt] != '\0') {
		while (str[cnt] < 33 && str[cnt] != '\0')
			++cnt;
		temp = cnt;
		while (str[cnt] > 32 && str[cnt] != ';')
			++cnt;
		_act->setName().push_back(std::string(&str[temp], &str[cnt]));
		while (str[cnt] < 33 && str[cnt] != '\0')
			++cnt;
	}
}

void Config_parser::_methods_filling_loc(char const *str)
{
	int temp;
	int cnt;

	cnt = 0;
	while (str[cnt] > 32)
		++cnt;
	while (str[cnt] != ';' && str[cnt] != '\0') {
		while (str[cnt] < 33 && str[cnt] != '\0')
			++cnt;
		temp = cnt;
		while (str[cnt] > 32 && str[cnt] != ';')
			++cnt;
		_a_loc->_methods.push_back(std::string(&str[temp], &str[cnt]));
		if (_a_loc->_methods.back() != "GET" &&
		    _a_loc->_methods.back() != "POST" &&
		    _a_loc->_methods.back() != "DELETE" &&
		    _a_loc->_methods.back() != "HEAD" &&
		    _a_loc->_methods.back() != "OPTIONS" &&
		    _a_loc->_methods.back() != "PUT") {
			write(2, "Unknown HTTP method: ", 21);
			write(2, _a_loc->_methods.back().c_str(),
			      _a_loc->_methods.back().size());
			write(2, "\n", 1);
			return;
		}
		while (str[cnt] < 33 && str[cnt] != '\0')
			++cnt;
	}
}

void Config_parser::_methods_filling(char const *str)
{
	int temp;
	int cnt;
	std::string strin;

	if (_normal(_context(str), "location") && !_a_loc)
		return;
	cnt = 0;
	while (str[cnt] > 32)
		++cnt;
	while (str[cnt] != ';' && str[cnt] != '\0') {
		while (str[cnt] < 33 && str[cnt] != '\0')
			++cnt;
		temp = cnt;
		while (str[cnt] > 32 && str[cnt] != ';')
			++cnt;
		strin.assign(&str[temp], &str[cnt]);
		if (_a_loc)
			_a_loc->_methods.push_back(strin);
		else
			_act->setMethods().push_back(strin);
		if (strin != "GET" &&
		    strin != "POST" &&
		    strin != "HEAD" &&
		    strin != "OPTIONS" &&
		    strin != "DELETE" &&
		    strin != "PUT") {
			write(2, "Unknown HTTP method: ", 21);
			write(2, _act->setMethods().back().c_str(),
			      _act->setMethods().back().size());
			write(2, "\n", 1);
			return;
		}
		while (str[cnt] < 33 && str[cnt] != '\0')
			++cnt;
	}
}

void Config_parser::_pars_error_pages(char const *str)
{
	std::string temp_path;
	int cnt;
	int temp;
	bool ins;
	size_t pos;
	int err_n;

	cnt = 11; //lenght of "error_pages"
	ins = false;
	if (!_normal(_context(str), "server")) {
		write(2, "error_pages directive must be inside server\n", 45);
		_error = ERR_PAGES;
		return;
	}
	while (str[cnt] != '{' && str[cnt] != '\0') {
		if ((str[cnt] < 48 || str[cnt] > '9') && str[cnt] > 32) {
			write(2, "error_pages directive must containes intergers\n", 48);
			_error = ERR_PAGES;
			return;
		}
		++cnt;
	}
	if (str[cnt] == '{')
		temp = ++cnt;
	else {
		write(2, "error_pages directive must containes intergers\n", 48);
		_error = ERR_PAGES;
		return;
	}
	while (str[cnt] != '}' && str[cnt] != '\0')
		++cnt;
	temp_path.assign(&str[temp], &str[cnt]);
	pos = temp_path.find_last_of("^");
	cnt = 11; //lenght of "error_pages"
	temp = 0;
	err_n = 0;
	while (str[cnt] != '{' && str[cnt] != '\0') {
		if (str[cnt] > 32 && ins == false) {
			err_n = atoi(&str[cnt]);
			ins = true;
			temp = cnt;
			_act->setErr_location()[err_n] = temp_path;
		} else if (str[cnt] < 33 && ins == true) {
			if (pos != std::string::npos)
				_act->setErr_location()[err_n].replace(pos, 1, &str[temp], cnt - temp);
			ins = false;
		}
		++cnt;
	}
	if (err_n == 0)
		_act->setErr_location()[err_n] = temp_path;
	if (_act->setErr_location().empty())
		_act->setErr_location()[0] = temp_path;
}

bool Config_parser::_check_file(std::string const &file)
{
	struct stat stats;

	if (!_check_path(file))
		return false;
	if (stat(file.c_str(), &stats)) {
		write(2, "Can't accept file: ", 19);
		write(2, file.c_str(), file.size());
		write(2, "\n", 1);
		return false;
	}
	return (true);
}

bool Config_parser::_check_dir(std::string const &dir)
{
	struct stat stats;

	if (!_check_path(dir))
		return false;
	if (stat(dir.c_str(), &stats)) {
		write(2, "Can't accept folder: ", 21);
		write(2, dir.c_str(), dir.size());
		write(2, "\n", 1);
		return false;
	}
	if (!S_ISDIR(stats.st_mode)) {
		write(2, dir.c_str(), dir.size());
		write(2, " is not folder\n", 15);
		return false;
	}
	return true;
}

bool Config_parser::_check_path(std::string const &path)
{
	int cnt;
	char const *str;
	int up;

	up = 0;
	cnt = _main_folder.size();
	str = path.c_str();
	while (str[cnt] != '\0' && up > -1) {
		if (str[cnt] == '.' && str[cnt + 1] == '.' && \
                        str[cnt - 1] == '/')
			--up;
		if (str[cnt] == '/')
			++up;
		if (cnt > 3 && str[cnt] == '/' && str[cnt - 1] == '.'\
 && str[cnt - 2] == '.' && str[cnt - 3] == '/')
			--up;
		++cnt;
	}
	if (up < 0) {
		write(2, path.c_str(), path.size());
		write(2, " is illegal path\n", 18);
		return false;
	}
	return true;
}

bool Config_parser::_check_location(config_unit &pars)
{
	bool ok;
	std::map<std::string, location_unit>::iterator it;
	std::map<std::string, location_unit>::iterator end;

	ok = true;
	end = pars.setLocation().end();
	it = pars.setLocation().begin();
	if (pars.setLocation().empty()) {
		write(2, "Server is empty\n", 17);
		return false;
	}
	if (!it->first.empty()) {
		write(2, "Server ", 7);
		write(2, pars.setListen().begin()->str.c_str(), \
            pars.setListen().begin()->str.size());
		write(2, " doesn't have default location\n", 32);
		return (false);
	}
	while (it != end && ok) {
		if (it != pars.setLocation().begin() && it->second._abs_path[0] == '.' \
 && it->second._abs_path[1] == '/') {
			it->second._abs_path.replace(0, 1 + (pars.setLocation().begin()\
->second._abs_path[pars.setLocation().begin()->second._abs_path.\
            size() - 1] == '/'), pars.setLocation().begin()->second._abs_path.\
            c_str(), pars.setLocation().begin()->second._abs_path.size());
		} else if (it->second._abs_path[0] == '/')
			it->second._abs_path.replace(0, 1, \
                            _main_folder.c_str(), _main_folder.size());
		else if (it == pars.setLocation().begin() && \
                                        it->second._abs_path[0] == '.') {
			it->second._abs_path.replace(0, 2, \
                            _main_folder.c_str(), _main_folder.size());
			if (it->second._abs_path[it->second._abs_path.size() - 1] != '/')
				it->second._abs_path += '/';
		} else
			it->second._abs_path = _main_folder + it->second._abs_path;
		ok = _check_dir(it->second._abs_path);
//		if (it->second._autoindex == 0)
//			it->second._autoindex = pars.getAutoindex();

		if (it->second._autoindex == 2)
			it->second._autoindex = 0;
		if (it->second._methods.empty())
			it->second._methods = pars.getMethods();
		if (it->second._def_file.empty())
			it->second._def_file = pars.getDefaultFile();
		_check_storage(pars, it->second);
		if (it->second._body_size == 0)
			it->second._body_size = pars.setMax_client_body();
		++it;
	}
	return (ok);
}

void Config_parser::_check_methods()
{
	_act->setMethods().push_back("GET");
	_act->setMethods().push_back("HEAD");
}

bool Config_parser::_check_cgi_loc(config_unit &pars)
{
	if (pars.setCGI_loc().empty()) {
		pars.setCGI_loc() = pars.setLocation().begin()->second._abs_path;
		return true;
	}
	if (pars.setCGI_loc()[0] == '.' && pars.setCGI_loc()[1] == '/')
		pars.setCGI_loc().replace(0, 1 + \
            (pars.setLocation().begin()->second._abs_path[pars.setLocation().\
            begin()->second._abs_path.size() - 1] == '/'), pars.setLocation().\
            begin()->second._abs_path.c_str(), pars.setLocation().begin()\
->second._abs_path.size());
	else if (pars.setCGI_loc()[0] == '/')
		pars.setCGI_loc().replace(0, 1, _main_folder.c_str(), \
                                                        _main_folder.size());
	else
		pars.setCGI_loc().insert(0, _main_folder);
	return (_check_dir(pars.setCGI_loc()));
}

bool Config_parser::_check_err_loc(config_unit &pars)
{
	bool ok;
	std::map<int, std::string>::iterator it;
	std::map<int, std::string>::iterator end;

	ok = true;
	end = pars.setErr_location().end();
	it = pars.setErr_location().begin();
	while (it != end && ok) {
		if (it->second[0] == '.' && it->second[1] == '/')
			it->second.replace(0, 1 + (pars.setLocation().begin()->second.\
            _abs_path[pars.setLocation().begin()->second._abs_path.size() - 1]\
 == '/'), pars.setLocation().begin()->second._abs_path.c_str(), \
                        pars.setLocation().begin()->second._abs_path.size());
		else if (it->second[0] == '/')
			it->second.replace(0, 1, _main_folder.c_str(), _main_folder.size());
		else
			it->second = _main_folder + it->second;
		ok = _check_file(it->second);
		++it;
	}
	it = pars.setErr_location().find(0);
	if (it == pars.setErr_location().end())
		pars.setErr_location()[0] = pars.setLocation().begin()->\
                                                        second._abs_path;
	return (ok);
}

bool Config_parser::_check_doubling_server_name\
(std::list<std::string> &it, \
                                        std::list<std::string> &act)
{
	std::list<std::string>::iterator it_b;
	std::list<std::string>::iterator it_e;
	std::list<std::string>::iterator act_b;
	std::list<std::string>::iterator act_e;

	it_b = it.begin();
	it_e = it.end();
	act_e = act.end();
	while (it_b != it_e) {
		act_b = act.begin();
		while (act_b != act_e) {
			if (*it_b == *act_b)
				return false;
			++act_b;
		}
		++it_b;
	}
	return true;
}

bool operator==(listen_unit const &lhs, listen_unit const &rhs)
{
	if (lhs.port != rhs.port)
		return false;
	return (lhs.str == rhs.str);
}

bool Config_parser::_check_doubling_server_listen\
(std::list<listen_unit> const &it, \
                                        std::list<listen_unit> const &act)
{
	std::list<listen_unit>::const_iterator it_b;
	std::list<listen_unit>::const_iterator it_e;
	std::list<listen_unit>::const_iterator act_b;
	std::list<listen_unit>::const_iterator act_e;

	it_b = it.begin();
	it_e = it.end();
	act_e = act.end();
	while (it_b != it_e) {
		act_b = act.begin();
		while (act_b != act_e) {
			if (*it_b == *act_b)
				return false;
			++act_b;
		}
		++it_b;
	}
	return true;
}

bool Config_parser::_check_doubling_server_two(std::list<config_unit>::iterator act)
{
	std::list<config_unit>::iterator it;
	std::list<config_unit>::iterator end;
	bool ok;

	ok = true;
	it = _conf.begin();
	end = _conf.end();
	while (it != end && ok) {
		if (it == act) {
			++it;
			continue;
		}
		if (!_check_doubling_server_name(it->setName(), act->setName()))
			ok = _check_doubling_server_listen(it->setListen(), act->setListen());
		++it;
	}
	if (!ok)
		write(2, "Doubled servers\n", 17);
	return (ok);
}

bool Config_parser::_check_doubling_server(void)
{
	std::list<config_unit>::iterator it;
	std::list<config_unit>::iterator end;
	bool ok;

	ok = true;
	it = _conf.begin();
	end = _conf.end();
	while (it != end && ok) {
		ok = _check_doubling_server_two(it);
		++it;
	}
	return (ok);
}

void Config_parser::_check_storage(config_unit &pars, location_unit &it)
{
	if (it._storage.empty()) {
		if (pars.setStorage_loc().empty())
			it._storage = pars.setLocation().begin()->second._abs_path;
		else if (pars.setStorage_loc()[0] == '/') {
			if (_main_folder[_main_folder.size() - 1] == '/')
				it._storage = _main_folder + \
                                     &pars.setStorage_loc()[1];
			else
				it._storage = _main_folder + pars.setStorage_loc();
		} else if (pars.setStorage_loc()[0] == '.') {
			if (pars.setLocation().begin()->\
                second._abs_path[pars.setLocation().begin()->\
                                second._abs_path.size() - 1] == '/')
				it._storage = pars.setLocation().begin()->second._abs_path\
 + &pars.setStorage_loc()[2];
			else
				it._storage = pars.setLocation().begin()->second._abs_path\
 + &pars.setStorage_loc()[1];
		} else {
			if (pars.setLocation().begin()->\
                second._abs_path[pars.setLocation().begin()->\
                    second._abs_path.size() - 1] == '/')
				it._storage = pars.setLocation().begin()->second._abs_path\
 + pars.setStorage_loc();
			else
				it._storage = pars.setLocation().begin()->second._abs_path\
 + "/" + pars.setStorage_loc();
		}

	} else if (it._storage[0] == '/') {
		if (_main_folder[_main_folder.size() - 1] == '/')
			it._storage = _main_folder + \
                                 &it._storage.c_str()[1];
		else
			it._storage = _main_folder + it._storage;
	} else if (it._storage[0] == '.') {
		it._storage.replace(0, 1, \
                pars.setLocation().begin()->second._abs_path);
	} else {
		if (pars.setLocation().begin()->\
            second._abs_path[pars.setLocation().begin()->\
                second._abs_path.size() - 1] == '/') {
			it._storage.replace(0, 1, \
                    pars.setLocation().begin()->second._abs_path);
		} else
			it._storage = pars.setLocation().begin()->second._abs_path + "/" \
 + it._storage;
	}

}

bool Config_parser::_check_parsed_data(void)
{
	std::list<config_unit>::iterator it;
	std::list<config_unit>::iterator end;
	bool ok;

	ok = true;
	it = _conf.begin();
	end = _conf.end();
	while (it != end) {
		_check_methods();
		if (it->getAutoindex() == 2)
			it->setAutoindex() = 0;
		if (it->setMax_client_body() == 0)
			it->setMax_client_body() = -1;
//		if (it->getDefaultFile().empty())
//			it->setDefaultFile("index.html");
		if (!(ok = _check_location(*it)))
			break;
		if (!(ok = _check_execs(*it)))
			break;
		if (!(ok = _check_err_loc(*it)))
			break;
		it->resort();
		++it;

	}
	return (ok);
}

bool Config_parser::init(char const *file_addr)
{
	_breck = 0;
	_act = NULL;
	if (!_open_file(file_addr))
		return false;
	if (!_recheck_breckts(&_file[0]))
		return false;
	if (!_conf.empty()) {
		if (!_check_parsed_data())
			_error = 100;
		if (!_check_doubling_server())
			_error = 100;
		_map_filling();
	}
	_file.clear();
	if (_act && !_error)
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
	while (conf_b != conf_e) {

		li_it_b = conf_b->setListen().begin();
		li_it_e = conf_b->setListen().end();
		while (li_it_b != li_it_e) {
			_ports[li_it_b->port].push_back(&*conf_b);
			++li_it_b;
		}
		++conf_b;
	}
}

Config_parser::Config_parser(void)
{
	_error = 0;
	_a_loc = NULL;
	_main_folder = CFP_MAIN_FOLDER;
}

Config_parser::~Config_parser(void) {}

Config_parser::Config_parser(Config_parser &for_copy)
{
	*this = for_copy;
}

Config_parser &Config_parser::operator=(Config_parser const &for_copy)
{
	_file = for_copy._file;
	return (*this);
}

std::list<config_unit> &Config_parser::getConf(void)
{
	return (_conf);
}

std::map<int, std::list<config_unit *> > const &Config_parser::getPortsMap(void)
{
	return (_ports);
}

std::vector<std::pair<std::string, int> > Config_parser::getAllListen(void)
{
	std::vector<std::pair<std::string, int> > allListen;
	std::list<config_unit>::iterator it;
	std::list<config_unit>::iterator end;
	std::pair<std::string, int> temp;
	std::list<listen_unit>::const_iterator it_l;
	std::list<listen_unit>::const_iterator end_l;

	allListen.reserve(_conf.size());
	it = _conf.begin();
	end = _conf.end();
	while (it != end) {
		it_l = it->getListen().begin();
		end_l = it->getListen().end();
		while (it_l != end_l) {
			if (it_l->str == "localhost")
				temp.first = CFP_LOCALHOST;
			else if (it_l->type == 'd')
				temp.first = it_l->str;
			temp.second = it_l->port;
			++it_l;
		}
		allListen.push_back(temp);
		++it;
	}
	std::sort(allListen.begin(), allListen.end());
	allListen.erase(std::unique(allListen.begin(), allListen.end()), allListen.end());
	return allListen;
}

config_unit *Config_parser::getServerConf(std::string host, int port)
{
	std::list<config_unit *>::iterator it;
	std::list<config_unit *>::iterator end;
	std::list<std::string>::const_iterator it_n;
	std::list<std::string>::const_iterator end_n;
	it = _ports[port].begin();
	end = _ports[port].end();
	while (it != end) {
		it_n = (*it)->getName().begin();
		end_n = (*it)->getName().end();
		while (it_n != end_n) {
			if (*it_n == host)
				return (*it);
			++it_n;
		}
		++it;
	}
	return (_ports[port].front());
}

void Config_parser::_pars_storage(char const *str)
{
	char const *context;
	int cnt;
	std::string temp;

	context = _context(str);
	if (!_normal(context, "server") && !_normal(context, "location")) {
		write(2, "File storage bad placed\n", 25);
		_error = BAD_CGI_LOC;
		return;
	}
	cnt = 12;//lenght of "file_storage"
	while (str[cnt] < 33 && str[cnt] != '\0' && str[cnt] != ';')
		++cnt;
	context = &str[cnt];
	while (str[cnt] > 32 && str[cnt] != ';')
		++cnt;
	temp.assign(context, &str[cnt]);
	if (_a_loc)
		_a_loc->_storage = temp;
	else
		_act->setFileStorage(temp);
	if (str[cnt] != ';') {
		while (str[cnt] < 33 && str[cnt] != '\0' && str[cnt] != ';')
			++cnt;
	}
	if (str[cnt] != ';') {
		_error = 13;
		write(2, "File storage too many arguments\n", 33);
	}
}

void Config_parser::_pars_python_exec(char const *str)
{
	char const *context;
	int cnt;
	std::string temp;

	context = _context(str);
	if (!_normal(context, "server")) {
		write(2, "python_exec bad placed\n", 25);
		_error = BAD_CGI_LOC;
		return;
	}
	cnt = 11;//lenght of "python_exec"
	while (str[cnt] < 33 && str[cnt] != '\0' && str[cnt] != ';')
		++cnt;
	context = &str[cnt];
	while (str[cnt] > 32 && str[cnt] != ';')
		++cnt;
	temp.assign(context, &str[cnt]);
	_act->setPythonExec(temp);
	if (str[cnt] != ';') {
		while (str[cnt] < 33 && str[cnt] != '\0' && str[cnt] != ';')
			++cnt;
	}
	if (str[cnt] != ';') {
		_error = 13;
		write(2, "python_exec too many arguments\n", 33);
	}
}

void Config_parser::_pars_php_exec(char const *str)
{
	char const *context;
	int cnt;
	std::string temp;

	context = _context(str);
	if (!_normal(context, "server")) {
		write(2, "php_exec bad placed\n", 25);
		_error = BAD_CGI_LOC;
		return;
	}
	cnt = 8;//lenght of "php_exec"
	while (str[cnt] < 33 && str[cnt] != '\0' && str[cnt] != ';')
		++cnt;
	context = &str[cnt];
	while (str[cnt] > 32 && str[cnt] != ';')
		++cnt;
	temp.assign(context, &str[cnt]);
	_act->setPHPexec(temp);
	if (str[cnt] != ';') {
		while (str[cnt] < 33 && str[cnt] != '\0' && str[cnt] != ';')
			++cnt;
	}
	if (str[cnt] != ';') {
		_error = 13;
		write(2, "php_exec too many arguments\n", 33);
	}
}

bool Config_parser::_check_execs(config_unit &it)
{
	std::string def_py("/usr/bin/python3");
	std::string def_php("/usr/bin/php");

	if (it.getPythonExec() != "DONT_USE") {
		if (it.getPythonExec().empty())
			it.setPythonExec(def_py);
		if (!_check_file(it.getPythonExec())) {
			write(2, "Cant find python exec at ", 25);
			write(2, it.getPythonExec().c_str(), it.getPythonExec().size());
			write(2, "\nIf you don't want use python scripts print\n \"python_exec DONT_USE;\" at .conf file\n", 83);
			return false;
		}
	}
	if (it.getPHPexec() != "DONT_USE") {
		if (it.getPHPexec().empty())
			it.setPHPexec(def_php);
		if (!_check_file(it.getPHPexec())) {
			write(2, "Cant find php exec at ", 22);
			write(2, it.getPHPexec().c_str(), it.getPHPexec().size());
			write(2, "\nIf you don't want use python scripts set\n \"php_exec DONT_USE;\" at .conf file\n", 83);
			return false;
		}
	}

	return true;
};
