
#include "WebServer.hpp"

int main() {

  WebServer &server = WebServer::Instance();

  server.setup();
  server.run();
  server.finish();

  return 0;
}
