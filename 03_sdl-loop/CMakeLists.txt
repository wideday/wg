cmake_minimum_required(VERSION 3.0)
project(03_sdl-loop)

if(MSVC)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++17")
else()
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic -Werror")
endif()

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/Debug)
add_executable(${PROJECT_NAME} main.cpp)
#target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_17)

find_library(SDL2_LIB libSDL2.a)

if (MINGW)        
    target_link_libraries(${PROJECT_NAME} 
               -lmingw32 
               -lSDL2main 
               "${SDL2_LIB}" # full path to libSDL2.a force to staticaly link with it
               -mwindows
               -Wl,--no-undefined
               -lm
               -ldinput8
               -ldxguid
               -ldxerr8
               -luser32 
               -lgdi32
               -lwinmm
               -limm32
               -lole32
               -loleaut32
               -lshell32
               -lversion
               -luuid
               -static-libgcc
               )
elseif(UNIX)    
    target_link_libraries(${PROJECT_NAME} 
               "${SDL2_LIB}" # full path to libSDL2.a force to staticaly link with it
               -Wl,--no-undefined
               -lm
               -ldl
               -lpthread
               -lrt
               )
endif()