bin_directory=$(pwd)


cd $bin_directory/build

rm -rf *
cmake ..
make
./example