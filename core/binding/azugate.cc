#include "azugate.h"
#include "../include/config.h"
#include "server.hpp"
#include "worker.hpp"
#include <spdlog/spdlog.h>

extern "C" {

void azugate_start() {
  using namespace azugate;
  auto io_context_ptr = boost::make_shared<boost::asio::io_context>();
  StartHealthCheckWorker(io_context_ptr);
  Server s(io_context_ptr, g_port);
  SPDLOG_INFO("azugate is listening on port {}", g_port);
  s.Run(io_context_ptr);
  SPDLOG_WARN("server exits");
}

void azugate_load_config(BindingServerConfig bindingConfig){
  auto config = azugate::ServerConfig{
    bindingConfig.port,
    bindingConfig.jwt_public_key_pem 
  };
  azugate::LoadConfig(config);
}

}