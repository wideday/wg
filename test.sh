#1
cd 01_hello-world
#'if [ ! -d "./build" ];
#    then mkdir build;
#fi'
mkdir build
cd build
cmake ..
make
cd ../bin/Debug
./01_hello-world

 #2_1
cd ../../../02_sdl-dynamic
#'if [ ! -d "./build" ];
#    then mkdir build;
#fi'
mkdir build
cd build
cmake ..
make
cd ../bin/Debug
./02_sdl-dynamic

#2_2
cd ../../../02_sdl-static
#'if [ ! -d "./build" ];
#    then mkdir build;
#fi'
mkdir build
cd build
cmake ..
make
cd ../bin/Debug
./02_sdl-static

#3_1
cd ../../../03_sdl-loop
#'if [ ! -d "./build" ];
#    then mkdir build;
#fi'
mkdir build
cd build
cmake ..
make

#3_2
cd ../03_sdl-loop-engine
#'if [ ! -d "./build" ];
#    then mkdir build;
#fi'
mkdir build
cd build
cmake ..
make

#3_3
cd ../03_sdl-loop-engine-dll
#'if [ ! -d "./build" ];
#    then mkdir build;
#fi'
mkdir build
cd build
cmake ..
make