#include "log.h"
#include "spdlog/spdlog.h"
#include <spdlog/async.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include "config.h"

namespace azugate {

// ref: https://github.com/gabime/spdlog/wiki/3.-Custom-formatting.
void InitLogger() {
  spdlog::init_thread_pool(kLoggerQueueSize, kLoggerThreadsCount);
  auto logger = spdlog::create_async<spdlog::sinks::stdout_color_sink_mt>(
      std::string(kDefaultLoggerName));
  spdlog::set_default_logger(logger);
  // production:
  // spdlog::set_pattern("[%^%l%$] %t | %D %H:%M:%S | %v");
  // with source file and line when debug:
  spdlog::set_pattern("[%^%l%$] %t | %D %H:%M:%S | %s:%# | %v");
  spdlog::set_level(spdlog::level::debug);
}

}