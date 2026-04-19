#include "config_wrapper.h"
#include "../core/include/config.h"
#include <string>

extern "C" {

void ConfigServer(ServerConfig *config) {
  azugate::ServerConfig sc;
  sc.jwt_public_key_pem = std::string(config->jwt_public_key_pem);
  sc.port = config->port;
  azugate::LoadServerConfig(sc);
}
}