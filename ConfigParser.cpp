//
// Created by mtriston on 03.05.2021.
//

#include "ConfigParser.hpp"

ConfigParser::ConfigParser(char *) {
  configs.push_back(Config("0.0.0.0", 4444));
}

std::vector<Config> &ConfigParser::getConfigs() {
  return configs;
}
