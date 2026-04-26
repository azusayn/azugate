#pragma once
#include "protocols.h"
#include <string>

namespace azugate {

// TODO: decouple routing logic from ConnectionInfo. This struct
// should only represent immutable connection metadata, and must
// not participate in routing strategy decisions directly.
struct ConnectionInfo {
  ProtocolType type;
  // IPv4/IPv6 address string.
  std::string downstream_address;
  uint16_t downstream_port = 0;
  std::string http_url;
  // access local file or remote endpoint.
  bool is_remote;
  bool operator==(const ConnectionInfo &other) const;
};

struct RouteAction {
  // used for round robin.
  size_t next_index;
  std::vector<ConnectionInfo> targets;

  void AddTarget(ConnectionInfo &&conn);

  void RemoveTarget(const ConnectionInfo &conn);

  std::optional<ConnectionInfo> GetNextTarget();

  bool Contains(const ConnectionInfo &conn) const;
};

// router API.
void AddPrefixMatchRoute(std::string source_url, std::string target_url,
                         bool is_local);

void AddPathMatchRoute(std::string source_url, std::string target_url,
                       bool is_local);

std::optional<ConnectionInfo> GetRouteTarget(const std::string source_url);
} // namespace azugate

namespace std {
template <> struct hash<azugate::ConnectionInfo> {
  size_t operator()(const azugate::ConnectionInfo &conn) const {
    size_t h1 = hash<azugate::ProtocolType>()(conn.type);
    size_t h2 = hash<string>()(conn.http_url);
    return h1 ^ (h2 << 1);
  }
};
} // namespace std