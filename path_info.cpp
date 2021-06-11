#include <string>
#include <algorithm>
#include <iostream>

#include <unistd.h>

size_t check_py(std::string const &uri)
{
	size_t res;
	res = uri.find(".py");
	if (res == std::string::npos)
		return res;
	if (uri[res + 3] == '?' || uri[res + 3] == '/' \
		|| uri[res + 3] == '\0')
		return res;
	return (std::string::npos);
}

size_t check_php(std::string const &uri)
{
	size_t res;
	res = uri.find(".php");
	if (res == std::string::npos)
		return res;
	if (uri[res + 4] == '?' || uri[res + 4] == '/' \
		|| uri[res + 4] == '\0')
		return res;
	return (std::string::npos);
}

size_t check_out(std::string const &uri)
{
	size_t res;
	res = uri.find(".out");
	if (res == std::string::npos)
		return res;
	if (uri[res + 4] == '?' || uri[res + 4] == '/' \
		|| uri[res + 4] == '\0')
		return res;
	return (std::string::npos);
}

std::string getPathInfo(std::string const &uri)
{
	size_t 	py;
	size_t	out;
	size_t	php;
	size_t	act;
	int		temp;
	const char *str;
		
	py = check_py(uri);
	out = check_php(uri);
	php = check_php(uri);
	act = std::min(py, out);
	act = std::min(act, php);

	if (uri[act + 2] == 'y')
		temp = act + 3;
	else
		temp = act + 4;
	if (uri[temp] == '/')
		return uri.substr(temp);
	str = uri.c_str();
	while (str[act] != '/')
		--act;
	return uri.substr(0, act);
} 
