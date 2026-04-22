#include "protocols.h"
#include <cstddef>

#include "router.h"

namespace std {
template <> struct hash<azugate::ConnectionInfo> {
  size_t operator()(const azugate::ConnectionInfo &conn) const {
    size_t h1 = hash<azugate::ProtocolType>()(conn.type);
    size_t h2 = hash<std::string>()(conn.http_url);
    return h1 ^ (h2 << 1);
  }
};
} // namespace std

namespace azugate {

bool ConnectionInfo::operator==(const ConnectionInfo &other) const {
  return false;
}

void RouterEntry::AddTarget(ConnectionInfo &&conn) {
  auto pred = [&](const ConnectionInfo &c) {
    return conn.address == c.address && conn.http_url == c.http_url &&
           conn.port == c.port && conn.type == c.type &&
           conn.remote == c.remote;
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