bin_directory=$(pwd)


cd $bin_directory/example/build

rm -rf *
cmake ..
make
./example