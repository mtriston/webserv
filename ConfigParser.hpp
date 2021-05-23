//
// Created by mtriston on 03.05.2021.
//

#ifndef WEBSERV__CONFIGPARSER_HPP_
#define WEBSERV__CONFIGPARSER_HPP_

#include "Config.hpp"
#include <vector>

class ConfigParser {
 public:
  ConfigParser(char *);
  std::vector<Config> &getConfigs();
 private:
  std::vector<Config> configs;
};

#endif //WEBSERV__CONFIGPARSER_HPP_
