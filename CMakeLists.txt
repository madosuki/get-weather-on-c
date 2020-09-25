cmake_minimum_required(VERSION 3.1)
project(get_openweather_data C)

# find_package(Threads REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(PkgConfig REQUIRED)

pkg_check_modules(JSONC REQUIRED json-c)
include_directories(${JSONC_INCLUDE_DIRS})

add_library(http_connector STATIC ./src/lib/http_connector/http_connector.c)
target_link_libraries(http_connector OpenSSL::SSL)

add_library(openweathermap_client STATIC ./src/lib/openweathermap_client/openweather.c)
target_link_libraries(openweathermap_client http_connector)
target_link_libraries(openweathermap_client ${JSONC_LIBRARIES})

add_executable(sample ./src/main/main.c)

target_link_libraries(sample http_connector)
target_link_libraries(sample openweathermap_client)
