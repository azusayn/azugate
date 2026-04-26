#include "../include/config.h"
#include <csignal>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <mutex>
#include <spdlog/async.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>
#include <string>
#include <unordered_set>
#include <utility>
#include <vector>

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

void IgnoreSignalPipe() {
#if defined(__linux__)
  // ignore SIGPIPE.
  struct sigaction sa{};
  sa.sa_handler = SIG_IGN;
  sigaction(SIGPIPE, &sa, nullptr);
#endif
}

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
// public key for verifying the token.
std::string g_jwt_public_key_pem;

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

void LoadConfig(ServerConfig config) {
  g_port = config.port;
  g_jwt_public_key_pem = config.jwt_public_key_pem;
  // g_ssl_crt = config[kYamlFieldCrt].as<std::string>();
  // g_ssl_key = config[kYamlFieldKey].as<std::string>();
};

} // namespace azugate