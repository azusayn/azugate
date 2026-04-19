#pragma once
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
  uint16_t port;
  const char *jwt_public_key_pem;
} ServerConfig;

void ConfigServer(ServerConfig *config);

#ifdef __cplusplus
}
#endif