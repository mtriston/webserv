//
// Created by mtriston on 02.05.2021.
//

#ifndef WEBSERV__UTILS_HPP_
#define WEBSERV__UTILS_HPP_

#include <string>
#include <algorithm>
#include <dirent.h>

std::string cutToken(std::string &str, std::string const &delim);

std::string ft_trim(std::string str);

long scanNumber(const char *str, int base);

std::string ft_tolower(std::string const &str);

std::string convertTime(const time_t *t);

bool isDirectory(std::string const &path);

bool isFileExists(std::string const &path);

#endif //WEBSERV__UTILS_HPP_
