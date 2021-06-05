 #include "ConfigUnit.hpp"
 
 location_unit & location_unit::operator=(location_unit const &cp)
 {
	 _abs_path = cp._abs_path;
	 _methods = cp._methods;
	 _autoindex = cp._autoindex;
	 _def_file = cp._def_file;
	 return (*this);
 }
 
listen_unit & listen_unit::operator=(listen_unit const & for_copy)
{
	str = for_copy.str;
	for (int i = 0; i < 4; i++)
		digit[i] = for_copy.digit[i];
	type = for_copy.type;
	port = for_copy.port;
	return (*this);
}

void ConfigUnit::setDefaultFile(std::string const &str)
{
	_def_file = str;
}

ConfigUnit& ConfigUnit::operator=(ConfigUnit const & for_copy)
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

std::list<std::string> & ConfigUnit::setName(void)
{
	return (_name);
};

std::map<std::string, location_unit> & ConfigUnit::setLocation(void)
{
	return (_location);
};

std::list <listen_unit>	& ConfigUnit::setListen(void)
{
	return (_listen);
};

std::list<std::string> & ConfigUnit::setMethods(void)
{
	return (_methods);
};

int & ConfigUnit::setError(void)
{
	return (_error);
};

std::string & ConfigUnit::setCGI_loc(void)
{
	return (_cgi_loc);
};

unsigned int & ConfigUnit::setMax_client_body(void)
{
	return (_max_client_body);
};

std::map<int,  std::string> & ConfigUnit::setErr_location(void)
{
	return (_err_location);
};

int & ConfigUnit::setAutoindex(void)
{
	return (_autoindex);
};

void ConfigUnit::setWorkers(int number)
{
	_workers = number;
};

std::list<std::string> const& ConfigUnit::getName(void) const
{
	return (_name);
};

std::map<std::string, location_unit> const& ConfigUnit::getLocation(void) const
{
	return (_location);
};

std::list <listen_unit>	const& ConfigUnit::getListen(void) const
{
	return (_listen);
};

std::list<std::string> const& ConfigUnit::getMethods(void) const
{
	return (_methods);
};

int ConfigUnit::getError(void) const
{
	return (_error);
};

std::string const& ConfigUnit::getCGI_loc(void) const
{
	return (_cgi_loc);
};

unsigned int ConfigUnit::getMax_client_body(void) const
{
	return (_max_client_body);
};

std::map<int,  std::string> const& ConfigUnit::getErr_location(void) const
{
	return (_err_location);
};

int ConfigUnit::getAutoindex(void) const
{
	return (_autoindex);
};

int ConfigUnit::getWorkers(void) const
{
	return (_workers);
}

std::string const& ConfigUnit::getDefaultFile(void)
{
	return (_def_file);
};

ConfigUnit::ConfigUnit(void)
{
	_autoindex = 0;
	_error = 0;
	_max_client_body = -1;
	_workers = 1;
};
ConfigUnit::~ConfigUnit(void){};

int  ConfigUnit::checkAutoindex(std::string const &path)
{
	return (_getLocation(path)->second._autoindex);
}

/*
принимает номер ошибки, отдаёт асолютный путь к файлу с ошибкой
если такого нет - отдаёт генерирует путь [корень/сайта/номер_ошибки.html]
*/
std::string const ConfigUnit::searchError_page(int err_num)
{
	std::map<int, std::string>::iterator it;
	
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
bool ConfigUnit::checkMethod(std::string const &method,\
			std::string const &path)
{
	std::list<std::string>::iterator it;
	std::list<std::string>::iterator end;
	std::map<std::string, location_unit>::iterator loc;
	
	loc = _getLocation(path);
	it = loc->second._methods.begin();
	end = loc->second._methods.end();
	while (it != end)
	{
		if (*it == method)
			return true;
		++it;
	}
	return (false);
}

void ConfigUnit::resort(void)
{
	_name.sort();
	_methods.sort();
}

std::string ConfigUnit::getServerPath(std::string const& path)
{
	int				cnt;
	std::string    	res;
	std::map<std::string, location_unit>::iterator it = _getLocation(path);
	
	res = it->second._abs_path;
	cnt = it->first.size();
	if (path[0] == '/' && res[res.size() - 1] == '/')
		++cnt;
	if (res[res.size() - 1] != '/')
		res.append("/");
	res.append(&path[cnt]);
	if (res[res.size() - 1] == '/')
		res.append(it->second._def_file);
	return res;
}

std::map<std::string, location_unit>::iterator\
							ConfigUnit::_getLocation(std::string const& path)
{
	std::map<std::string, location_unit>::iterator it;
	std::map<std::string, location_unit>::iterator end;
	
	it = _location.begin();
	end = _location.end();
	++it;
	while (it != end)
	{
		if (it->first.find(path.c_str(), 0, it->first.size()) == 0)
			return it;
		++it;
	}
	return _location.begin();
}
