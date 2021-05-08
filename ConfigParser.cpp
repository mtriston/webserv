//
// Created by mtriston on 03.05.2021.
//

#include "ConfigParser.hpp"

ConfigParser::ConfigParser(char *) {
  configs.push_back(Config("127.0.0.1", 4444));
  configs.push_back(Config("192.168.1.68", 4444));
}

std::vector<Config> const &ConfigParser::getConfigs() const {
  return configs;
}
