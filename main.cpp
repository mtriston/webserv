
#include "ServerCluster.hpp"

int main() {

  ServerCluster &cluster = ServerCluster::Instance();

  cluster.setup();
  cluster.run();
  cluster.finish();

  return 0;
}
