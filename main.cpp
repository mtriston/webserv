
#include "ServerCluster.hpp"
#include "ConfigParser.hpp"
#include <iostream>

int main(int argv, char **argc) {

  if (argv > 2) {
    std::cerr << "Invalid count of arguments!" << std::endl;
    return (1);
  }

  ConfigParser configParser(argc[1]);
  ServerCluster cluster;

  try {
    cluster.setup(configParser.getConfigs());
  } catch (std::exception &e) {
    std::cerr << "Initialization error: " << e.what() << std::endl;
    return (1);
  }

  cluster.run();

  return 0;
}
