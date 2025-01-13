cmake_minimum_required(VERSION 3.15)

project(TruckPlatooning LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/build/bin)

set(SOURCE_LEADING_TRUCK ${CMAKE_SOURCE_DIR}/src/main.cpp)
set(SOURCE_FOLLOWING_TRUCK ${CMAKE_SOURCE_DIR}/src/following.cpp)

file(GLOB_RECURSE SOURCES ${CMAKE_SOURCE_DIR}/src/*.cpp)
list(REMOVE_ITEM SOURCES ${SOURCE_LEADING_TRUCK} ${SOURCE_FOLLOWING_TRUCK})

include_directories(${CMAKE_SOURCE_DIR}/src)

add_executable(LeadingTruck ${SOURCE_LEADING_TRUCK} ${SOURCES})
add_executable(FollowingTruck ${SOURCE_FOLLOWING_TRUCK} ${SOURCES})

#Libraries
find_package(Threads REQUIRED)
find_package(spdlog REQUIRED)

target_link_libraries(LeadingTruck PRIVATE
    Threads::Threads 
    spdlog::spdlog
)

target_link_libraries(FollowingTruck PRIVATE
    Threads::Threads 
    spdlog::spdlog
)

#Compiler options
# if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
#     target_compile_options(LeadingTruck PRIVATE -Wall -Wextra -pedantic)
#     target_compile_options(FollowingTruck PRIVATE -Wall -Wextra -pedantic)
# endif()

add_compile_options(-Wall -Wextra -g)