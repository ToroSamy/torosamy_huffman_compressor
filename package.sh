echo "start generating out folder"

project_directory=$(pwd)
cd "$project_directory"

rm -rf out
mkdir out



cp -rf include out
cp -rf lib out
cp -rf run.sh out
cp -rf install.sh out
cp -rf example out
rm -rf out/example/build/*
rm -rf out/example/example.java
rm -rf out/example/example.torosamy
rm -rf out/example/output_example.java

echo "Please enter the version"
read version

zip_filename="torosamy_huffman_compressor-$version"

# 删除之前可能存在的同名压缩文件
rm -f "out/$zip_filename.tar.gz"

# 切换到 out 文件夹
cd out

# 将当前目录下的所有内容压缩到上一级目录的 zip 文件中
tar -czvf "../$zip_filename.tar.gz" .

cd ..
mv "$zip_filename.tar.gz" out

echo "Compressed file has been generated in out folder: $project_directory/out/$zip_filename.tar.gz"