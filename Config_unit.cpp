 #include "Config_unit.hpp"
 
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

std::list<std::string> & config_unit::setName(void)
{
	return (_name);
};

std::map<std::string, std::string> & config_unit::setLocation(void)
{
	return (_location);
};

std::list <listen_unit>	& config_unit::setListen(void)
{
	return (_listen);
};

std::list<std::string> & config_unit::setMethods(void)
{
	return (_methods);
};

int & config_unit::setError(void)
{
	return (_error);
};

std::string & config_unit::setCGI_loc(void)
{
	return (_cgi_loc);
};

unsigned int & config_unit::setMax_client_body(void)
{
	return (_max_client_body);
};

std::map<int,  std::string> & config_unit::setErr_location(void)
{
	return (_err_location);
};

bool & config_unit::setAutoindex(void)
{
	return (_autoindex);
};

void config_unit::setWorkers(int number)
{
	_workers = number;
};

std::list<std::string> const& config_unit::getName(void) const
{
	return (_name);
};

std::map<std::string, std::string> const& config_unit::getLocation(void) const
{
	return (_location);
};

std::list <listen_unit>	const& config_unit::getListen(void) const
{
	return (_listen);
};

std::list<std::string> const& config_unit::getMethods(void) const
{
	return (_methods);
};

int config_unit::getError(void) const
{
	return (_error);
};

std::string const& config_unit::getCGI_loc(void) const
{
	return (_cgi_loc);
};

unsigned int config_unit::getMax_client_body(void) const
{
	return (_max_client_body);
};

std::map<int,  std::string> const& config_unit::getErr_location(void) const
{
	return (_err_location);
};

bool config_unit::getAutoindex(void) const
{
	return (_autoindex);
};

int config_unit::getWorkers(void) const
{
	return (_workers);
}

config_unit::config_unit(void)
{
	_autoindex = false;
	_error = 0;
	_max_client_body = -1;
	_workers = 1;
};
config_unit::~config_unit(void){};

std::string const& config_unit::searchError_page(int err_num)
{
	std::map<int, std::string>::iterator it;
	
	it = _err_location.find(err_num);
	if (it != _err_location.end())
		return (it->second);
	return (_location[""]);
};

bool config_unit::checkMethod(std::string const &method)
{
	std::list<std::string>::iterator it;
	std::list<std::string>::iterator end;
	
	it = _methods.begin();
	end = _methods.end();
	while (it != end)
	{
		if (*it == method)
			return true;
		++it;
	}
	return (false);
};