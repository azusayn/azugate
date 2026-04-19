#pragma once
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

struct ServerConfig {
  uint16_t port;
  const char *jwt_public_key_pem;
};

void ConfigServer(ServerConfig *config);

#ifdef __cplusplus
}
#endif