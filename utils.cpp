//
// Created by mtriston on 02.05.2021.
//

#include <sys/stat.h>
#include "utils.hpp"

std::string cutToken(std::string &str, std::string const &delim)
{
	std::string token = str.substr(0, str.find(delim));
	if (!token.empty()) {
		str.erase(0, token.size() + delim.length());
	}
	return token;
}

std::string ft_tolower(std::string const &str)
{
	std::string result(str);
	std::transform(result.begin(), result.end(), result.begin(), ::tolower);
	return result;
}

std::string convertTime(const time_t *t)
{
	struct tm *timeinfo;
	char buffer[80] = {};

	timeinfo = gmtime(t);
	strftime(buffer, 80, "%a, %d %b %Y %T %Z", timeinfo);
	return std::string(buffer);
}

bool isDirectory(std::string const &path)
{
	struct stat statbuf = {};
	if (stat(path.c_str(), &statbuf) != 0)
		return false;
	bool res = S_ISDIR(statbuf.st_mode);
	return res;
}

bool isFileExists(const std::string &path)
{
	struct stat buffer = {};
	return (stat(path.c_str(), &buffer) == 0);
}