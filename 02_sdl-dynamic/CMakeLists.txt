cmake_minimum_required(VERSION 3.0)
project(02_sdl-dynamic)

if(MSVC)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++17")
else()
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic -Werror")
endif()

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/Debug)
add_executable(${PROJECT_NAME} main.cpp)

find_library(SDL2_LIB libSDL2.so)

if (MINGW)        
    target_link_libraries(${PROJECT_NAME} 
               -lmingw32 
               -lSDL2main                
               -lSDL2
               -mwindows
               )
elseif(UNIX)    
    target_link_libraries(${PROJECT_NAME} 
               -lSDL2
               "${SDL2_LIB}"
               )
endif()
#target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_17)