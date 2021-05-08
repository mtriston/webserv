//
// Created by mtriston on 02.05.2021.
//

#ifndef WEBSERV__UTILS_HPP_
#define WEBSERV__UTILS_HPP_

#include <string>
#include <algorithm>

std::string cut_next_token(std::string &str, std::string const &delim);

int ft_atoi(std::string const &str);

std::string ft_tolower(std::string const &str);

char		*ft_itoa(int n);

#endif //WEBSERV__UTILS_HPP_