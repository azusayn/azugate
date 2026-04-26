#pragma once
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void azugate_start();

typedef struct {
  uint16_t port;
  const char *jwt_public_key_pem;
} BindingServerConfig;

void azugate_load_config(BindingServerConfig);

#ifdef __cplusplus
}
#endif
