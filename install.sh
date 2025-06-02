bin_directory=$(pwd)


cd $bin_directory

sudo rm -rf build/*
cd build
cmake ..
make
cd ..

sudo rm -rf lib/*
sudo cp build/libtorosamy_huffman_compressor.so lib/

sudo rm -rf /opt/torosamy_huffman_compressor



sudo mkdir /opt/torosamy_huffman_compressor
sudo cp -rf $bin_directory/lib /opt/torosamy_huffman_compressor
sudo cp -rf $bin_directory/include /opt/torosamy_huffman_compressor




echo 'Done! '
