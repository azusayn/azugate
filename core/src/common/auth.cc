#include "auth.h"
#include <jwt-cpp/jwt.h>
#include <spdlog/spdlog.h>
#include <string>

namespace azugate {
namespace utils {

bool VerifyToken(const std::string &token, const std::string &secret) {
  try {
    auto decoded_token = jwt::decode(token);
    auto verifier = jwt::verify()
                        .allow_algorithm(jwt::algorithm::hs256{secret})
                        .with_issuer(std::string(kDftTokenIssuer));
    verifier.verify(decoded_token);
    SPDLOG_DEBUG("validate token successfully");
    return true;
  } catch (const std::exception &e) {
    SPDLOG_WARN("error verifying token: {}", e.what());
    return false;
  }
}

} // namespace utils
} // namespace azugate