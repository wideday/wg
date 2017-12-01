cmake_minimum_required(VERSION 3.0)
project(01_hello-world)

if(MSVC)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++17")
else()
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic -Werror")
endif()

#set(CMAKE_BUILD_TYPE Release)
#set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/Release)
#add_executable(${PROJECT_NAME} main.cpp)
#target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_17)

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/Debug)
add_executable(${PROJECT_NAME} main.cpp)
#target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_17)