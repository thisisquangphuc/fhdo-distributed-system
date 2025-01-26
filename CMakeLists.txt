cmake_minimum_required(VERSION 3.15)

project(TruckPlatooning LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/build/bin)

set(SOURCE_LEADING_TRUCK ${CMAKE_SOURCE_DIR}/src/main.cpp)
set(SOURCE_FOLLOWING_TRUCK ${CMAKE_SOURCE_DIR}/src/following.cpp)

file(GLOB_RECURSE SOURCES 
    ${CMAKE_SOURCE_DIR}/src/*.cpp
    ${CMAKE_SOURCE_DIR}/src/*.c
    ${CMAKE_SOURCE_DIR}/src/*.cc
)
list(REMOVE_ITEM SOURCES ${SOURCE_LEADING_TRUCK} ${SOURCE_FOLLOWING_TRUCK})

include_directories(${CMAKE_SOURCE_DIR}/src)
include_directories(${CMAKE_SOURCE_DIR}/src/communication)
include_directories(${CMAKE_SOURCE_DIR}/src/control)

# target_include_directories(PlatoonUnitTests PUBLIC ${CMAKE_SOURCE_DIR}/src/communication)
# target_include_directories(ThreadPool PUBLIC ${CMAKE_SOURCE_DIR}/src/utils)

# Add the dotenv.c file to the source files
# set(DOTENV_SOURCE ${CMAKE_SOURCE_DIR}/src/libs/dotenv/dotenv.c)

# Enable testing
enable_testing()
# Add a subdirectory for tests
add_subdirectory(tests)


add_executable(LeadingTruck ${SOURCE_LEADING_TRUCK} ${SOURCES})
add_executable(FollowingTruck ${SOURCE_FOLLOWING_TRUCK} ${SOURCES})

#Libraries
find_package(Threads REQUIRED)
find_package(spdlog REQUIRED)
find_package(nlohmann_json REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(GTest REQUIRED)

# Include the googletest headers
# include_directories(${GTEST_INCLUDE_DIRS})

# Link libraries
target_link_libraries(LeadingTruck PRIVATE
    Threads::Threads 
    spdlog::spdlog
    nlohmann_json::nlohmann_json
    OpenSSL::SSL
)

target_link_libraries(FollowingTruck PRIVATE
    Threads::Threads 
    spdlog::spdlog
    nlohmann_json::nlohmann_json
    OpenSSL::SSL
)

#Compiler options
# if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
#     target_compile_options(LeadingTruck PRIVATE -Wall -Wextra -pedantic)
#     target_compile_options(FollowingTruck PRIVATE -Wall -Wextra -pedantic)
# endif()

add_compile_options(-Wall -Wextra -g)