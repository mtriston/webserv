#include "Config_unit.hpp"
#include "utils.hpp"

location_unit &location_unit::operator=(location_unit const &cp)
{
	_abs_path = cp._abs_path;
	_methods = cp._methods;
	_autoindex = cp._autoindex;
	_def_file = cp._def_file;
	_redirect = cp._redirect;
	_body_size = cp._body_size;
	return (*this);
}

listen_unit &listen_unit::operator=(listen_unit const &for_copy)
{
	str = for_copy.str;
	for (int i = 0; i < 4; i++)
		digit[i] = for_copy.digit[i];
	type = for_copy.type;
	port = for_copy.port;
	return (*this);
}

void config_unit::setDefaultFile(std::string const &str)
{
	_def_file = str;
}

void config_unit::setFileStorage(std::string const &str)
{
	_storage = str;
}

config_unit &config_unit::operator=(config_unit const &for_copy)
{
	_name = for_copy._name;
	_location = for_copy._location;
	_listen = for_copy._listen;
	_methods = for_copy._methods;
	_error = for_copy._error;
	_cgi_loc = for_copy._cgi_loc;
	_max_client_body = for_copy._max_client_body;
	_err_location = for_copy._err_location;
	_autoindex = for_copy._autoindex;
	return (*this);
}

std::list<std::string> &config_unit::setName(void)
{
	return (_name);
};

std::map<std::string, location_unit> &config_unit::setLocation(void)
{
	return (_location);
};

std::list<listen_unit> &config_unit::setListen(void)
{
	return (_listen);
};

std::list<std::string> &config_unit::setMethods(void)
{
	return (_methods);
};

int &config_unit::setError(void)
{
	return (_error);
};

std::string &config_unit::setCGI_loc(void)
{
	return (_cgi_loc);
};

size_t &config_unit::setMax_client_body(void)
{
	return (_max_client_body);
};

std::map<int, std::string> &config_unit::setErr_location(void)
{
	return (_err_location);
};

std::string &config_unit::setStorage_loc(void)
{
	return (_storage);
};

int &config_unit::setAutoindex(void)
{
	return (_autoindex);
};

void config_unit::setWorkers(int number)
{
	_workers = number;
};

std::list<std::string> const &config_unit::getName(void) const
{
	return (_name);
};

std::map<std::string, location_unit> const &config_unit::getLocation(void) const
{
	return (_location);
};

std::list<listen_unit> const &config_unit::getListen(void) const
{
	return (_listen);
};

std::list<std::string> const &config_unit::getMethods(void) const
{
	return (_methods);
};

int config_unit::getError(void) const
{
	return (_error);
};

std::string const &config_unit::getCGI_loc(void) const
{
	return (_cgi_loc);
};

size_t config_unit::getMax_client_body(std::string const &path)
{

	return (_getLocation(path)->second._body_size);
};

std::map<int, std::string> const &config_unit::getErr_location(void) const
{
	return (_err_location);
};

int config_unit::getAutoindex(void) const
{
	return (_autoindex);
};

int config_unit::getWorkers(void) const
{
	return (_workers);
}

std::string const &config_unit::getDefaultFile(void)
{
	return (_def_file);
};

config_unit::config_unit(void)
{
	_autoindex = 0;
	_error = 0;
	_max_client_body = -1;
	_workers = 1;
};

config_unit::~config_unit(void) {};

int config_unit::checkAutoindex(std::string const &path)
{
	return (_getLocation(path)->second._autoindex);
}

/*
принимает номер ошибки, отдаёт асолютный путь к файлу с ошибкой
если такого нет - отдаёт генерирует путь [корень/сайта/номер_ошибки.html]
*/
std::string const config_unit::searchError_page(int err_num)
{
	std::map<int, std::string>::iterator it;

	if (_err_location.empty())
		return (std::string());
	it = _err_location.find(err_num);
	if (it != _err_location.end())
		return (it->second);
	std::stringstream temp;
	temp << err_num;
	return (_err_location.begin()->second + temp.str() + ".html");
};

/*
checkMethod(метод, путь_в_запросе(неизменённый, как в первой строке))
функция отвечает возможено ли применить такой метод к такому запросу
*/
bool config_unit::checkMethod(std::string const &method, \
            std::string const &path)
{
	std::list<std::string>::iterator it;
	std::list<std::string>::iterator end;
	std::map<std::string, location_unit>::iterator loc;

	loc = _getLocation(path);
	it = loc->second._methods.begin();
	end = loc->second._methods.end();
	while (it != end) {
		if (*it == method)
			return true;
		++it;
	}
	return (false);
}

void config_unit::resort(void)
{
	_name.sort();
	_methods.sort();
}

/*
требует адрес из запроса, возвращает абсолютный путь к файлу на сервере
*/
std::string config_unit::getServerPath(std::string const &path)
{
	size_t cnt;
	std::string res;
	std::map<std::string, location_unit>::iterator it = _getLocation(path);

	if (_location.empty())
		return (std::string());
	res = it->second._abs_path;
	cnt = it->first.size();
//	if (path[0] == '/' && res[res.size() - 1] == '/')
//		++cnt;
	if (res[res.size() - 1] != '/')
		res.append("/");
	if (cnt < path.size())
		res.append(&path[cnt]);
	if (res[res.size() - 1] == '/') {
		res.append(it->second._def_file);
	} else if (isDirectory(res)) {
		res.append("/" + it->second._def_file);
	}
	return res;
}

std::map<std::string, location_unit>::iterator\
 config_unit::_getLocation(std::string const &path)
{
	std::map<std::string, location_unit>::iterator it;
	std::map<std::string, location_unit>::iterator end;

	it = _location.begin();
	end = _location.end();
	++it;
	while (it != end) {
		if (_pathComp(path.c_str(), it->first.c_str()))
			return it;
		++it;
	}
	return _location.begin();
}

/*
требует путь из запроса, отвечает будет ли он перенаправлен
*/
bool config_unit::checkRedirect(std::string const &path)
{
	if (_getLocation(path)->second._redirect.second.empty())
		return false;
	return true;
};

/*
требует путь из запроса, возвращает пару: код редиректа и адрес для редиректа
*/
std::pair<int, std::string> config_unit::getRedirectPath(std::string\
 const &path)
{
	std::map<std::string, location_unit>::iterator it;
	std::pair<int, std::string> res;
	int cnt;

	it = _getLocation(path);
	cnt = it->second._redirect.second.size() - 1;
	if (cnt < 0)
		return (std::pair<int, std::string>());
	if (it->second._redirect.second[cnt] != '^')
		return (_getLocation(path)->second._redirect);
	res.first = _getLocation(path)->second._redirect.first;
	res.second.assign(&it->second._redirect.second[0], \
                                &it->second._redirect.second[cnt]);
	cnt = it->first.size();
	if (path[0] == '/' && res.second[res.second.size() - 1] == '/')
		++cnt;
	if (res.second[res.second.size() - 1] != '/')
		res.second.append("/");
	else
		--cnt;
	res.second.append(&path[cnt]);
	return (res);
}

std::string config_unit::getCGI_Path(std::string const &path)
{
	if (_cgi_loc.empty())
		return (getServerPath(path));
	std::string res = _location[""]._abs_path;
	if (res[res.size() - 1] != '/' && _cgi_loc[0] != '/')
		res.append("/");
	res.append(_cgi_loc);
	res.append(&path[path.find_last_of('/')]);
	return res;
}

bool config_unit::_pathComp(char const *path, char const *iter)
{
	int cnt_p;
	int cnt_i;

	cnt_p = 0;
	cnt_i = 0;
	if (path[0] == '/')
		cnt_p = 1;
	if (iter[0] == '/')
		cnt_i = 1;
	while (path[cnt_p] == iter[cnt_i]) {
		if (path[cnt_p] == '\0' || iter[cnt_i] == '\0')
			break;
		++cnt_i;
		++cnt_p;
	}
	if (iter[cnt_i] == '\0' || (iter[cnt_i] == '/' && iter[cnt_i + 1] == '\0')) {
		if (path[cnt_p] == '/')
			return true;
		else if (path[cnt_p] == '\0')
			return true;
		else if (path[cnt_p - 1] == '/')
			return true;
	}
	return false;
}

std::string config_unit::getUploadPath(std::string const &path)
{
	size_t cnt;
	std::string res;
	std::map<std::string, location_unit>::iterator it = _getLocation(path);

	if (_location.empty())
		return (std::string());
	res = it->second._storage;
	cnt = it->first.size();
	if (res[res.size() - 1] != '/')
		res.append("/");
	if (cnt < path.size())
		res.append(&path[cnt]);
	return res;
}

std::string config_unit::getPathFromLocation(const std::string &path)
{
	size_t cnt;
	std::string res;
	std::map<std::string, location_unit>::iterator it = _getLocation(path);

	if (_location.empty())
		return (std::string());
	res = it->second._abs_path;
	cnt = it->first.size();
	if (res[res.size() - 1] != '/')
		res.append("/");
	if (cnt < path.size())
		res.append(&path[cnt]);
	return res;
}

void config_unit::setPythonExec(std::string const &str)
{
	_python_path = str;
};

void config_unit::setPHPexec(std::string const &str)
{
	_php_path = str;
};

std::string const &config_unit::getPythonExec(void)
{
	return _python_path;
};

std::string const &config_unit::getPHPexec(void)
{
	return _php_path;
};
