#!/usr/bin/sh

hash_code=$( git -C . --no-pager log -1 --pretty=format:%H)
file_name="$hash_code".tar.gz
wget https://github.com/libsese/sese/archive/"$file_name"
archive_hash_code=$(sha512sum $file_name | awk '{ print $1 }')
echo "commit hash $hash_code"
echo "archive hash $archive_hash_code"