

// TODO:
// ref: https://www.envoyproxy.io/docs/envoy/latest/start/sandboxes.
// memmory pool optimaization.
// int main(int argc, char *argv[]) {
//   auto io_context_ptr = boost::make_shared<boost::asio::io_context>();
//   azugate::Server s(io_context_ptr, azugate::g_port);
//   SPDLOG_INFO("azugate is listening on port {}", azugate::g_port);
//   s.Run(io_context_ptr);
//   SPDLOG_WARN("server exits");

//   return 0;
// }
#include <iostream>
extern "C" {

  void azugate_start() { std::cout << "Hello, azugate!\n"; }

}