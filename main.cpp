
#include "ServerCluster.hpp"

int main(int argv, char **argc) {

  std::vector<Config> configs;
  if (argv == 1) {
    configs = ConfigParser().getConfigs();
    //TODO: парсинг дефолтного конфига
  } else if (argv == 2) {
    configs = ConfigParser().getConfigs();
    //TODO: парсинг файла в argc[1]
  } else {
    std::cerr << "Invalid count of arguments!" << std::endl;
    return (1);
  }
  ServerCluster &cluster = ServerCluster::Instance();

  cluster.setup(configs);
  cluster.run();
  cluster.finish();

  return 0;
}
