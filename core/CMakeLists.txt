cmake_minimum_required(VERSION 3.25.0)
project(azugate)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_BUILD_TYPE Debug)

find_package(spdlog REQUIRED)
find_package(Boost COMPONENTS asio thread system url CONFIG)
find_package(OpenSSL REQUIRED)
find_package(jwt-cpp REQUIRED)
find_package(ZLIB REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)

set(CORE_INCLUDES_DIR include binding)
set(THIRD_PARTY_INCLUDES_DIR "third_party")
file(GLOB_RECURSE CORE_SOURCES
    "src/*.cc"
    "binding/azugate.cc"
) 
file(GLOB_RECURSE THIRD_PARTY_SOURCES
    "third_party/*.cc"
    "third_party/*.c"
)

# library for all the apps.
add_library(azugate_core STATIC
${CORE_SOURCES}
${THIRD_PARTY_SOURCES}
)

target_include_directories(azugate_core PUBLIC
${CORE_INCLUDES_DIR} 
${THIRD_PARTY_INCLUDES_DIR}
)

target_link_libraries(azugate_core
spdlog::spdlog
ZLIB::ZLIB
Boost::asio
Boost::thread
Boost::url
nlohmann_json::nlohmann_json
Boost::system
${OPENSSL_LIBRARIES}
${jwt-cpp_LIBRARIES}
)
