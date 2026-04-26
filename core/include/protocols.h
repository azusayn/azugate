#ifndef __PROTOCOL_DETECTOR_H
#define __PROTOCOL_DETECTOR_H

#include <boost/asio/ip/tcp.hpp>
#include <boost/smart_ptr/shared_ptr.hpp>
#include <string_view>
namespace azugate {
constexpr std::string_view kProtocolTypeHttp = "http";
constexpr std::string_view kProtocolTypeTcp = "tcp";
constexpr std::string_view kProtocolTypeUdp = "udp";
constexpr std::string_view kProtocolTypeGrpc = "grpc";
constexpr std::string_view kProtocolTypeWebSocket = "websocket";
constexpr std::string_view kProtocolTypeUnknown = "";

using ProtocolType = std::string_view;

} // namespace azugate
#endif