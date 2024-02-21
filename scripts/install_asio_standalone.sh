#!/bin/sh

target_directory="__asio__"

if [ -d "$target_directory" ]; then
    echo "Error: The directory '$target_directory' already exists."
    exit 1
fi

clone_url="https://github.com/chriskohlhoff/asio"
git clone --depth 1 "$clone_url" "$target_directory" || { echo "Error: Cloning failed."; exit 1; }

cd "$target_directory/asio" || { echo "Error: Changing directory failed."; exit 1; }

./autogen.sh || { echo "Error: Running autogen.sh failed."; cd ../..; rm -rf "$target_directory"; exit 1; }
./configure || { echo "Error: Running configure failed."; cd ../..; rm -rf "$target_directory"; exit 1; }
make install -j 4 || { echo "Error: Compilation failed."; cd ../..; rm -rf "$target_directory"; exit 1; }

cd ../..

rm -rf "$target_directory"

echo "Success: asio installed and compiled successfully!"
