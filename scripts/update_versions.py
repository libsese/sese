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


import argparse
import re


def modify_field_in_line(data, line_pattern, value_pattern, new_value):
    lines = data.splitlines()
    new_lines = []

    for line in lines:
        if re.search(line_pattern, line):
            line = re.sub(value_pattern, str(new_value), line)
        new_lines.append(line)

    result = "\n".join(new_lines)
    if not result.endswith("\n"):
        result += "\n"

    return result


def modify_field_in_file(filename, line_pattern, value_pattern, new_value):
    with open(filename) as file:
        data = file.read()
    modified_data = modify_field_in_line(data, line_pattern, value_pattern, new_value)
    with open(filename, 'w') as file:
        file.write(modified_data)


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--version', type=str, required=True, help="New version")
    args = parser.parse_args()

    files = [
        ["vcpkg.json", "version", "\d+\.\d+\.\d+"],
        ["CMakeLists.txt", r'^\s{4}VERSION', "\d+\.\d+\.\d+"],
        ["sese/manifest.json", "version", "\d+\.\d+\.\d+"],
    ]
    for file in files:
        modify_field_in_file(file[0], file[1], file[2], args.version)


if __name__ == '__main__':
    main()
