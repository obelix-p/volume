cmake_minimum_required(VERSION 3.10)
project(MarketDataReader)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find required packages
find_package(CURL REQUIRED)
find_package(nlohmann_json 3.2.0 QUIET)

# Include directories
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories(${CURL_INCLUDE_DIRS})

# Source files
set(SOURCES
    src/main.cpp
    src/market_data_fetcher.cpp
    src/data_storage.cpp
    src/config.cpp
)

# Create executable
add_executable(market_data_reader ${SOURCES})

# Link libraries
target_link_libraries(market_data_reader ${CURL_LIBRARIES} pthread)

# If nlohmann_json is available, use it; otherwise use header-only
if(nlohmann_json_FOUND)
    target_link_libraries(market_data_reader nlohmann_json::nlohmann_json)
endif()

print(STATUS "Build configuration complete")
