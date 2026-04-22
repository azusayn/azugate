#pragma once
#include "protocols.h"

namespace azugate {
struct ConnectionInfo {
  ProtocolType type;
  // currently IPv4.
  std::string address;
  uint16_t port = 0;
  std::string http_url;
  // access local file or remote endpoint.
  bool remote;
  bool operator==(const ConnectionInfo &other) const;
};

void AddRoute(ConnectionInfo &&source, ConnectionInfo &&target);

std::optional<ConnectionInfo> GetTargetRoute(const ConnectionInfo &source);

size_t GetRouterTableSize();

struct RouterEntry {
  // used for round robin.
  size_t next_index;
  std::vector<ConnectionInfo> targets;

  void AddTarget(ConnectionInfo &&conn);

  void RemoveTarget(const ConnectionInfo &conn);

  std::optional<ConnectionInfo> GetNextTarget();

  bool Contains(const ConnectionInfo &conn) const;
};

} // namespace azugate