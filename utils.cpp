//
// Created by mtriston on 02.05.2021.
//

#include "utils.hpp"

std::string cut_next_token(std::string &str, std::string const &delim) {
  std::string token = str.substr(0, str.find(delim));
  if (!token.empty()) {
    str.erase(0, token.length() + delim.length());
  }
  return token;
}