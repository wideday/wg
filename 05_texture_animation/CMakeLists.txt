cmake_minimum_required(VERSION 3.2)
project(05_texture_animation)
#set(CMAKE_CXX_STANDARD 11)

if(MSVC)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++11")
else()
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic -Werror")
endif()

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/Debug)

add_library(engine SHARED engine.cpp)
target_compile_features(engine PUBLIC cxx_std_11)

if(WIN32)   
    target_compile_definitions(engine PRIVATE "-DNS_DECLSPEC=__declspec(dllexport)")
endif(WIN32)

find_library(SDL2_LIB NAMES SDL2)

if (MINGW)
    target_link_libraries(engine 
               -lmingw32 
               -lSDL2main 
               -lSDL2
               -mwindows
               -lopengl32
               -lglew32
               -lglu32
               )
elseif(UNIX)
    target_link_libraries(engine
               -lSDL2
               -lGL
               -lGLEW
               -lGLU
               )
endif()

add_executable(${PROJECT_NAME}_game game.cpp)
target_compile_features(${PROJECT_NAME}_game PUBLIC cxx_std_11)

target_link_libraries(${PROJECT_NAME}_game engine)