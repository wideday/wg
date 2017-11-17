cmake_minimum_required(VERSION 3.0)
project(02_sdl-dynamic)

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
               )
endif()