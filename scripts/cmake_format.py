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


import os
import subprocess


def find_cmake_files(paths):
    cmake_files = []

    for path in paths:
        for root, _, files in os.walk(path):
            for file in files:
                if file.endswith('.cmake'):
                    cmake_files.append(os.path.join(root, file))
                if file == 'CMakeLists.txt':
                    cmake_files.append(os.path.join(root, file))

    return cmake_files


def format_cmake_files(cmake_files):
    for cmake_file in cmake_files:
        subprocess.run(['cmake-format', '-c', '.cmake-format.yaml', '-i', cmake_file])


if __name__ == "__main__":
    paths = [
        "sese",
        "cmake"
    ]

    cmake_files = find_cmake_files(paths)
    cmake_files.append('CMakeLists.txt')
    format_cmake_files(cmake_files)

