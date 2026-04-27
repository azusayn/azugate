#include "router.h"
#include "protocols.h"
#include <spdlog/spdlog.h>

namespace azugate {
// http router
std::unordered_map<std::string, RouteAction> g_exact_routes;
std::vector<std::pair<ConnectionInfo, RouteAction>> g_prefix_routes;

// perfect match and prefix match.
bool ConnectionInfo::operator==(const ConnectionInfo &other) const {
  if (type != other.type) {
    return false;
  }
  if (type == kProtocolTypeTcp) {
    return downstream_address == other.downstream_address;
  }
  return (type == kProtocolTypeHttp || type == kProtocolTypeWebSocket) &&
         http_url == other.http_url;
}

// TODO: decouple RouterEntry from load balancing strategy.
// Introduce an abstraction layer (e.g. strategy/policy) so
// Round-Robin implementation is transparent.
void RouteAction::AddTarget(ConnectionInfo &&conn) {
  auto pred = [&](const ConnectionInfo &c) {
    return conn.http_url == c.http_url && conn.is_remote == c.is_remote;
  };
  auto it = std::find_if(targets.begin(), targets.end(), pred);
  if (it == targets.end()) {
    targets.emplace_back(conn);
  }
  return;
}

void RouteAction::RemoveTarget(const ConnectionInfo &conn) {
  auto it = std::remove(targets.begin(), targets.end(), conn);
  if (it != targets.end()) {
    targets.erase(it, targets.end());
    if (next_index >= targets.size() && !targets.empty()) {
      next_index %= targets.size();
    }
  }
}

std::optional<ConnectionInfo> RouteAction::GetNextTarget() {
  if (targets.empty()) {
    return std::nullopt;
  }
  ConnectionInfo &result = targets[next_index];
  next_index = (next_index + 1) % targets.size();
  return result;
}

bool RouteAction::Contains(const ConnectionInfo &conn) const {
  return std::find(targets.begin(), targets.end(), conn) != targets.end();
}

inline bool matchPrefix(const std::string &source_url,
                        const std::string &prefix) {
  return source_url.starts_with(prefix);
}

void AddPrefixMatchRoute(std::string source_url, std::string target_url,
                         bool is_local) {
  ConnectionInfo target;
  target.http_url = target_url;
  target.is_remote = !is_local;
  target.type = kProtocolTypeHttp;
  for (auto &route : g_prefix_routes) {
    if (matchPrefix(source_url, route.first.http_url)) {
      route.second.AddTarget(std::move(target));
      return;
    }
  }
  RouteAction router_entry{};
  router_entry.AddTarget(std::move(target));
  ConnectionInfo source;
  source.http_url = source_url;
  g_prefix_routes.emplace_back(std::move(source), std::move(router_entry));
  return;
}

void AddPathMatchRoute(std::string source_url, std::string target_url,
                       bool is_local) {
  ConnectionInfo target{
      .type = kProtocolTypeHttp,
      .http_url = target_url,
      .is_remote = !is_local,
  };
  auto it = g_exact_routes.find(source_url);
  if (it != g_exact_routes.end()) {
    it->second.AddTarget(std::move(target));
    return;
  }
  RouteAction router_entry;
  router_entry.AddTarget(std::move(target));
  g_exact_routes.emplace(source_url, std::move(router_entry));
  return;
}

// TODO: prefix tree.
std::optional<ConnectionInfo> GetRouteTarget(const std::string source_url) {
  // exact match.
  auto it = g_exact_routes.find(source_url);
  if (it != g_exact_routes.end()) {
    return it->second.GetNextTarget();
  }

  // prefix match: find the longest matching prefix.
  RouteAction *longest_match = nullptr;
  size_t longest_len = 0;

  for (auto &route : g_prefix_routes) {
    const std::string &prefix = route.first.http_url;
    // starts with same prefix.
    if (matchPrefix(source_url, prefix)) {
      continue;
    }
    // ensure we matched on a path boundary:
    // e.g. prefix="/api" should not match "/api2"
    size_t after = prefix.size();
    if (after == source_url.size() || source_url[after] == '/' ||
        prefix.back() == '/') {
      continue;
    }
    if (after <= longest_len) {
      continue;
    }
    longest_len = after;
    longest_match = &route.second;
  }

  if (!longest_match) {
    return std::nullopt;
  }
  return longest_match->GetNextTarget();
}

} // namespace azugate