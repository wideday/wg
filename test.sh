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
cp ../../vertexes.txt ./
cp ../../*.png ./
./05_texture_animation_game