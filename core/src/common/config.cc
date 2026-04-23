#include "../../include/config.h"
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <mutex>
#include <optional>
#include <spdlog/spdlog.h>
#include <string>
#include "router.h"
#include <unordered_set>
#include <utility>
#include <vector>
#include <yaml-cpp/node/parse.h>
#include <yaml-cpp/yaml.h>   
#include <spdlog/async.h>
#include <spdlog/sinks/stdout_color_sinks.h>


namespace azugate {
uint16_t g_port = 443;
uint16_t g_azugate_admin_port = 50051;
std::string g_path_config_file;
std::unordered_set<std::string> g_ip_blacklist;
bool g_enable_http_compression = false;
bool g_enable_https = false;
// TODO: mTLS.
std::string g_ssl_crt;
std::string g_ssl_key;
std::mutex g_config_mutex;

// ref: https://github.com/gabime/spdlog/wiki/3.-Custom-formatting.
void InitLogger() {
  spdlog::init_thread_pool(kLoggerQueueSize, kLoggerThreadsCount);
  auto logger = spdlog::create_async<spdlog::sinks::stdout_color_sink_mt>(std::string(kDefaultLoggerName));
  spdlog::set_default_logger(logger);
  // production:
  // spdlog::set_pattern("[%^%l%$] %t | %D %H:%M:%S | %v");
  // with source file and line when debug:
  spdlog::set_pattern("[%^%l%$] %t | %D %H:%M:%S | %s:%# | %v");
  spdlog::set_level(spdlog::level::debug);
}

void IgnoreSignalPipe() {
#if defined(__linux__)
  // ignore SIGPIPE.
  struct sigaction sa{};
  sa.sa_handler = SIG_IGN;
  sigaction(SIGPIPE, &sa, nullptr);
#endif
}

// router
std::unordered_map<ConnectionInfo, RouterEntry> g_exact_routes;
std::vector<std::pair<ConnectionInfo, RouterEntry>> g_prefix_routes;
// token.
std::string g_jwt_public_key_pem;
// rate limitor
bool g_enable_rate_limiter = false;
size_t g_num_token_per_sec = 100;
size_t g_num_token_max = 1000;
// io
size_t g_num_threads = 4;
// healthz.
std::vector<std::string> g_healthz_list;

// external auth
bool g_http_external_authorization = false;
std::string g_external_auth_domain;
std::string g_external_auth_client_id;
std::string g_external_auth_client_secret;
std::string g_external_auth_callback_url;

std::string GetConfigPath() {
  std::lock_guard<std::mutex> lock(g_config_mutex);
  return g_path_config_file;
};

void SetConfigFilePath(const std::string &path) {
  std::lock_guard<std::mutex> lock(g_config_mutex);
  g_path_config_file = path;
}

std::unordered_set<std::string> GetIpBlackList() {
  std::lock_guard<std::mutex> lock(g_config_mutex);
  return g_ip_blacklist;
};

void AddBlacklistIp(const std::string &&ip) {
  std::lock_guard<std::mutex> lock(g_config_mutex);
  // TODO: return more details.
  g_ip_blacklist.insert(ip);
}

void RemoveBlacklistIp(const std::string &&ip) {
  // TODO: return more details.
  g_ip_blacklist.erase(ip);
}

bool GetHttpCompression() {
  std::lock_guard<std::mutex> lock(g_config_mutex);
  return g_enable_http_compression;
}

void SetHttpCompression(bool http_compression) {
  g_enable_http_compression = http_compression;
}

void SetHttps(bool https) { g_enable_https = https; }

bool GetHttps() { return g_enable_https; }

void SetEnableRateLimitor(bool enable) { g_enable_rate_limiter = enable; };
bool GetEnableRateLimitor() { return g_enable_rate_limiter; };

void ConfigRateLimitor(size_t num_token_max, size_t num_token_per_sec) {
  std::lock_guard<std::mutex> lock(g_config_mutex);
  if (num_token_max > 0) {
    g_num_token_max = num_token_max;
  }
  if (num_token_per_sec > 0) {
    g_num_token_per_sec = num_token_per_sec;
  }
}
// return g_num_token_max and g_num_token_per_sec.
std::pair<size_t, size_t> GetRateLimitorConfig() {
  std::lock_guard<std::mutex> lock(g_config_mutex);
  return std::pair<size_t, size_t>(g_num_token_max, g_num_token_per_sec);
}

void AddHealthzList(std::string &&addr) {
  std::lock_guard<std::mutex> lock(g_config_mutex);
  g_healthz_list.emplace_back(addr);
}

const std::vector<std::string> &GetHealthzList() {
  std::lock_guard<std::mutex> lock(g_config_mutex);
  return g_healthz_list;
}

inline bool prefixMatchEqual(const ConnectionInfo &source_conn_info,
                             const ConnectionInfo &rule_conn_info) {
  return source_conn_info.http_url.starts_with(rule_conn_info.http_url.substr(
             0, rule_conn_info.http_url.find('*'))) &&
         source_conn_info.type == rule_conn_info.type;
}

void AddRoute(ConnectionInfo &&source, ConnectionInfo &&target) {
  if (source.http_url.find("*") != std::string::npos) {
    SPDLOG_DEBUG("add prefix match rule: {} -> {}", source.http_url,
                 target.http_url);
    for (auto &route : g_prefix_routes) {
      auto http_url = route.first.http_url;
      if (prefixMatchEqual(source, route.first)) {
        route.second.AddTarget(std::move(target));
        return;
      }
    }
    RouterEntry router_entry{};
    router_entry.AddTarget(std::move(target));
    g_prefix_routes.emplace_back(std::move(source), std::move(router_entry));
    return;
  }
  // exact match.
  auto er_it = g_exact_routes.find(source);
  if (er_it != g_exact_routes.end()) {
    er_it->second.AddTarget(std::move(target));
    return;
  }
  RouterEntry router_entry{};
  router_entry.AddTarget(std::move(target));
  g_exact_routes.emplace(std::move(source), std::move(router_entry));
  return;
}

std::optional<ConnectionInfo> GetTargetRoute(const ConnectionInfo &source) {
  std::lock_guard<std::mutex> lock(g_config_mutex);
  // exact match first.
  auto it = g_exact_routes.find(source);
  if (it != g_exact_routes.end() && !it->second.targets.empty()) {
    auto next_target = it->second.GetNextTarget();
    return next_target;
  }
  // prefix match.
  for (auto &route : g_prefix_routes) {
    if (!prefixMatchEqual(source, route.first)) {
      continue;
    }
    auto target = route.second.GetNextTarget();
    auto &target_url = target->http_url;
    if (target_url.size() >= 2 &&
        target_url.compare(target_url.size() - 2, 2, "/*") == 0) {
      std::string target_prefix = target_url.substr(0, target_url.size() - 2);
      std::string suffix = source.http_url;
      if (suffix.find(target_prefix) == 0) {
        suffix = suffix.substr(target_prefix.size());
      }
      if (!target_prefix.empty() && target_prefix.back() != '/' &&
          (suffix.empty() || suffix.front() != '/')) {
        target_prefix += '/';
      }
      target->http_url = target_prefix + suffix;
    }
    return target;
  }
  SPDLOG_WARN("no path found for: {}", source.http_url);
  return std::nullopt;
}

size_t GetRouterTableSize() { return g_exact_routes.size(); }

void LoadServerConfig(const ServerConfig &config) {
  g_port = config.port;
  g_jwt_public_key_pem = config.jwt_public_key_pem;
  // g_ssl_crt = config[kYamlFieldCrt].as<std::string>();
  // g_ssl_key = config[kYamlFieldKey].as<std::string>();
};

} // namespace azugate