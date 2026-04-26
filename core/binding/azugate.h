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

void azugate_add_prefix_match_route(const char *source_url,
                                    const char *target_url, int is_local);

void azugate_add_path_match_route(const char *source_url,
                                  const char *target_url, int is_local);

#ifdef __cplusplus
}
#endif
