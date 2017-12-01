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

 #2
cd ../../02_sdl-dynamic
#'if [ ! -d "./build" ];
#    then mkdir build;
#fi'
mkdir build
cd build
cmake ..
make
cd ../bin/Debug
./02_sdl-dynamic

#3
cd ../../02_sdl-static
#'if [ ! -d "./build" ];
#    then mkdir build;
#fi'
mkdir build
cd build
cmake ..
make
cd ../bin/Debug
./02_sdl-static