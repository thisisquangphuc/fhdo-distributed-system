cmake_minimum_required(VERSION 3.15)

project(TruckPlatooningTesting LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

find_package(GTest REQUIRED)
# if(GTest_FOUND)
#     message(STATUS "GoogleTest found: ${GTest_INCLUDE_DIRS}")
# else()
#     message(FATAL_ERROR "GoogleTest not found.")
# endif()

# Link root project's libraries (ensure the root CMakeLists.txt is included)
# add_subdirectory(${CMAKE_SOURCE_DIR}/.. ${CMAKE_BINARY_DIR}/../build)
find_package(Threads REQUIRED)
find_package(spdlog REQUIRED)
find_package(nlohmann_json REQUIRED)
find_package(OpenSSL REQUIRED)

# Add your test executable
add_executable(PlatoonUnitTests command_test.cpp following_test.cpp)

set(SERVER_SOURCES
    ${CMAKE_SOURCE_DIR}/src/communication/platoon_server.cpp
    ${CMAKE_SOURCE_DIR}/src/communication/platoon_client.cpp
    ${CMAKE_SOURCE_DIR}/src/control/trucks_manager.cpp
    ${CMAKE_SOURCE_DIR}/src/control/event_manager.cpp
    ${CMAKE_SOURCE_DIR}/src/control/following_truck.cpp
    ${CMAKE_SOURCE_DIR}/src/control/monitor.cpp
    ${CMAKE_SOURCE_DIR}/src/app.cc
    ${CMAKE_SOURCE_DIR}/src/utils/logger.cpp
    # Add other related source files if necessary
)

# Link  project libraries
target_include_directories(PlatoonUnitTests PRIVATE 
    ${CMAKE_SOURCE_DIR}/../src
)

target_include_directories(PlatoonUnitTests PUBLIC ${CMAKE_SOURCE_DIR}/src)

# Print CMAKE_SOURCE_DIR
message(STATUS "CMAKE_SOURCE_DIR: ${CMAKE_SOURCE_DIR}")

target_sources(PlatoonUnitTests PRIVATE ${SERVER_SOURCES})
# Link Google Test libraries and pthread
target_link_libraries(PlatoonUnitTests PRIVATE
    GTest::gtest 
    GTest::gtest_main 
    Threads::Threads
    spdlog::spdlog
    nlohmann_json::nlohmann_json
    OpenSSL::SSL
)

# Add a test target
# add_test(NAME PlatoonUnitTests COMMAND PlatoonUnitTests)
include(GoogleTest)
gtest_discover_tests(PlatoonUnitTests)
