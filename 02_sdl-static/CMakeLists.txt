cmake_minimum_required(VERSION 3.0)
project(02_sdl-static)

add_executable(${PROJECT_NAME} main.cpp)

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