#include "router.h"

namespace azugate {

// perfect match and prefix match.
bool ConnectionInfo::operator==(const ConnectionInfo &other) const {
  if (type != other.type) {
    return false;
  }
  if (type == ProtocolTypeTcp) {
    return downstream_address == other.downstream_address;
  }
  return (type == ProtocolTypeHttp || type == ProtocolTypeWebSocket) &&
         http_url == other.http_url;
}

// TODO: decouple RouterEntry from load balancing strategy.
// Introduce an abstraction layer (e.g. strategy/policy) so 
// Round-Robin implementation is transparent.
void RouterEntry::AddTarget(ConnectionInfo &&conn) {
  auto pred = [&](const ConnectionInfo &c) {
    return conn.downstream_address == c.downstream_address && conn.http_url == c.http_url &&
           conn.downstream_port == c.downstream_port && conn.type == c.type &&
           conn.is_remote == c.is_remote;
  };
  auto it = std::find_if(targets.begin(), targets.end(), pred);
  if (it == targets.end()) {
    targets.emplace_back(conn);
  }
  return;
}

// TODO: exact match & prefix match.
void RouterEntry::RemoveTarget(const ConnectionInfo &conn) {
  auto it = std::remove(targets.begin(), targets.end(), conn);
  if (it != targets.end()) {
    targets.erase(it, targets.end());
    if (next_index >= targets.size() && !targets.empty()) {
      next_index %= targets.size();
    }
  }
}

std::optional<ConnectionInfo> RouterEntry::GetNextTarget() {
  if (targets.empty()) {
    return std::nullopt;
  }
  ConnectionInfo &result = targets[next_index];
  next_index = (next_index + 1) % targets.size();
  return result;
}

bool RouterEntry::Contains(const ConnectionInfo &conn) const {
  return std::find(targets.begin(), targets.end(), conn) != targets.end();
}

} // namespace azugate