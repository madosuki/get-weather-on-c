cmake_minimum_required(VERSION 3.1)
project(get_openweather_data C)

# find_package(Threads REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(PkgConfig REQUIRED)

pkg_check_modules(JSONC REQUIRED json-c)
include_directories(${JSONC_INCLUDE_DIRS})

add_library(http_connector STATIC ./src/lib/http_connector/http_connector.c)

add_executable(sample ./src/main/main.c ./src/main/openweather.c)

target_link_libraries(sample http_connector)
target_link_libraries(sample OpenSSL::SSL)
target_link_libraries(sample ${JSONC_LIBRARIES})
