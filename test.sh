#5_2
cd 05_texture_animation
#'if [ ! -d "./build" ];
#    then mkdir build;
#fi'
mkdir build
cd build
cmake ..
make
cd ../bin/Debug
./05_vertex-morphing_game