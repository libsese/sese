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
import sys
from datetime import datetime

is_stream_mode = False
is_resource_mode = False
name = "nameless"

string_usage = (
    "res_pack.py [-r|-s] name files\n"
    "             -r     to a resource package\n"
    "             -s     to a resource stream\n"
    "             name   name of the package or stream\n"
    "             files  set source directory for package or set single source file for stream"
)


def print_status():
    print("is_stream_mode:{}".format(is_stream_mode))
    print("is_resource_mode:{}".format(is_resource_mode))
    print("name:{}".format(name))


def get_datetime():
    return datetime.now().strftime("%Y-%m-%d %H:%M:%S")


def escape_special_chars(ch: bytes) -> bytes:
    mapping = {
        "\a": "\\a",
        "\b": "\\b",
        "\t": "\\t",
        "\n": "\\n",
        "\v": "\\v",
        "\f": "\\f",
        "\r": "\\r",
        '"': '\\"',
        "\\": "\\\\",
    }
    for key, value in mapping.items():
        if ch == key.encode("utf-8"):
            return value.encode("utf-8")
    return ch


def write_binary(output, data, length):
    for i in range(0, length):
        # 方案1
        # output.write(('\\x{:02x}'.format(data[i])).encode('utf-8'))
        # 方案2
        # 如果比特是可打印的，则直接输出字符，否则转义
        # if 32 <= data[i] <= 126:
        #     output.write(data[i:i + 1])
        # else:
        #     output.write('\\x{:02x}'.format(data[i]).encode('utf-8'))
        # 方案3
        # 转义字符为字符串的转义形式
        if 7 <= data[i] <= 13 or data[i] == 34 or data[i] == 92:
            output.write(escape_special_chars(data[i : i + 1]))
        else:
            output.write(data[i : i + 1])


def get_files(base_path: str) -> list:
    files = list()
    for filepath, dirnames, filenames in os.walk(base_path):
        for filename in filenames:
            files.append(os.path.join(filepath, filename))
    return files


def write_res_add(file, output) -> bool:
    src = open(file, "rb")
    if not src:
        print("failed to open {}".format(file))
        return False

    output.write(
        '    SESE_ADD_RES("{}", {},\n'.format(
            file.replace("\\", "/"), os.path.getsize(file)
        ).encode("utf-8")
    )

    first = True
    while True:
        data = src.read(128)
        if not data:
            break
        elif first:
            first = False

        output.write('        "'.encode("utf-8"))
        write_binary(output, data, len(data))
        output.write('"\n'.encode("utf-8"))

    if first:
        output.write('        ""\n'.encode("utf-8"))

    output.write("    )\n".encode("utf-8"))

    src.close()
    return True


def write_res(directory) -> bool:
    output = open(name + ".rc.h", "wb")

    if not output:
        print("failed to open ".format(name + ".rc.h"))
        return False

    output.write(
        (
            "/// \\file {}\n"
            "/// \\date {}\n"
            "/// \\author res_pack.py\n"
            "/// \\brief resource header file\n"
            "\n"
            "#pragma once\n"
            "\n"
            '#include "sese/res/Marco.h"\n'
            "\n"
        )
        .format(name + ".rc.h", get_datetime())
        .encode("utf-8")
    )

    output.write("SESE_DEF_RES_MANAGER({})\n".format(name).encode("utf-8"))

    files = get_files(directory)
    for file in files:
        print("packaging {}".format(file))
        if not write_res_add(file, output):
            print("failed to packaging {}".format(file))
            exit(0)

    output.write("SESE_DEF_RES_MANAGER_END({})".format(name).encode("utf-8"))
    output.close()
    return True


def write_stream(file) -> bool:
    class_name = "".join(c if c.isalnum() else "_" for c in file)
    if not class_name[0].isalpha():
        class_name = "_" + class_name

    output = open(name + ".rc.h", "wb")
    if not output:
        print("failed to open ".format(name + ".rc.h"))
        return False

    output.write(
        (
            "/// \\file {}\n"
            "/// \\date {}\n"
            "/// \\author res_pack.py\n"
            "/// \\brief resource header file\n"
            "\n"
            "#pragma once\n"
            "\n"
            '#include "sese/res/Marco.h"\n'
            "\n"
        )
        .format(name + ".rc.h", get_datetime())
        .encode("utf-8")
    )

    output.write(
        "SESE_DEF_RES_STREAM({}, {},\n".format(
            class_name, os.path.getsize(file)
        ).encode("utf-8")
    )

    src = open(file, "rb")
    if not src:
        print("failed to open {}".format(file))
        output.close()
        return False

    first = True
    while True:
        data = src.read(128)
        if not data:
            break
        elif first:
            first = False

        output.write('    "'.encode("utf-8"))
        write_binary(output, data, len(data))
        output.write('"\n'.encode("utf-8"))

    if first:
        output.write('""\n'.encode("utf-8"))

    output.write(")\n".encode("utf-8"))

    output.close()
    return True


def main():
    global is_stream_mode
    global is_resource_mode
    global name

    if len(sys.argv) == 1:
        print(string_usage)
        exit(0)
    if len(sys.argv) < 3:
        print(string_usage)
        exit(0)

    if sys.argv[1][0] == "-":
        if sys.argv[1][1] == "r":
            is_resource_mode = True
        elif sys.argv[1][1] == "s":
            is_stream_mode = True

    name = sys.argv[2]

    files = sys.argv
    del files[0:3]

    if len(files) != 1:
        print(string_usage)
        exit(0)

    if not os.path.exists(files[0]):
        print('"{}" does not existed.'.format(files[0]))
        exit(0)

    if is_resource_mode and os.path.isdir(files[0]):
        write_res(files[0])
    elif is_stream_mode and os.path.isfile(files[0]):
        write_stream(files[0])
    else:
        print("Unsupported type of resource package or stream.")
        pass


if __name__ == "__main__":
    main()
