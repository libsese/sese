#  Copyright 2024 libsese
#
#  Licensed under the Apache License, Version 2.0 (the "License");
#  you may not use this file except in compliance with the License.
#  You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
#  Unless required by applicable law or agreed to in writing, software
#  distributed under the License is distributed on an "AS IS" BASIS,
#  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#  See the License for the specific language governing permissions and
#  limitations under the License.
#
#  Licensed under the Apache License, Version 2.0 (the "License");
#  you may not use this file except in compliance with the License.
#  You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
#  Unless required by applicable law or agreed to in writing, software
#  distributed under the License is distributed on an "AS IS" BASIS,
#  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#  See the License for the specific language governing permissions and
#  limitations under the License.

import os
import sys
import shutil


# Test the current directory
def test_root_path(path: str):
    fileA = open(path + "FileA.txt", "wt")
    fileB = open(path + "FileB.txt", "a")
    fileA.write("Hello")
    fileB.close()
    fileA.close()
    shutil.copyfile(path + "FileA.txt", path + "FileC.txt")
    os.rename(path + "FileB.txt", path + "FileD.txt")


# Test subdirectories
def test_sub_path(path: str):
    subdir = path + "subdir/"
    os.makedirs(subdir)
    subA = open(subdir + "subA.txt", "wt")
    subB = open(subdir + "subB.txt", "a")
    subA.write("hello")
    subB.close()
    subA.close()


# Copy the subdirectory file to the root directory
# copy $src/subA.txt to $dst/FileE.txt
def test_copy_cross_path(src: str, dst: str):
    shutil.copyfile(src + "subA.txt", dst + "FileE.txt")


# cleanup
def cleanup(path: str):
    os.remove(path + "FileA.txt")
    os.remove(path + "FileC.txt")
    os.remove(path + "FileD.txt")
    os.remove(path + "FileE.txt")
    os.remove(path + "subdir/" + "subA.txt")
    os.remove(path + "subdir/" + "subB.txt")
    os.rmdir(path + "subdir")


def main():
    # path = 'C:/temp/'
    # path = '/tmp/'
    path = sys.argv[1]
    if not os.path.exists(path):
        print("creating test dir")
        os.mkdir(path)
    print("changing files")
    test_root_path(path)
    test_sub_path(path)
    test_copy_cross_path(path + "subdir/", path)
    cleanup(path)
    print("done")


if __name__ == "__main__":
    main()
