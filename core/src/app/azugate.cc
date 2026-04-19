#include <spdlog/spdlog.h>

#include "config.h"
#include "server.hpp"
#include "worker.hpp"
#include <cxxopts.hpp>

// TODO:
// ref: https://www.envoyproxy.io/docs/envoy/latest/start/sandboxes.
// memmory pool optimaization.
int main(int argc, char *argv[]) {
  using namespace azugate;

  auto io_context_ptr = boost::make_shared<boost::asio::io_context>();

  StartHealthCheckWorker(io_context_ptr);

  Server s(io_context_ptr, g_port);
  SPDLOG_INFO("azugate is listening on port {}", g_port);

  s.Run(io_context_ptr);

  SPDLOG_WARN("server exits");

  return 0;
}
