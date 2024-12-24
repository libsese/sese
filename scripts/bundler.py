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
import json
import os
import queue
import string
import subprocess
from pathlib import Path
from typing import Dict, Tuple, Optional

# fmt: off
note = """ // Please do not modify this file manually
// This file is generated by Bundler.py
"""
# fmt: on


def parse_file(filename: str) -> Tuple[Dict[str, str], Optional[str]]:
    with open(filename, "r") as file:
        data = json.load(file)

    if "icon" in data:
        return data["binaries"], data["icon"]
    else:
        return data["binaries"], None


def sanitize_filename(name: str) -> str:
    invalid_chars = "-+*/@%&!?=^~:;,."
    sanitized = "".join(
        (
            c
            if c not in invalid_chars and c not in string.whitespace and c.isprintable()
            else "_"
        )
        for c in name
    )
    return sanitized


def get_last_16_chars(s: str) -> str:
    return s[-16:] if len(s) >= 16 else s


def write_windows_rc(
    binaries: Dict[str, str], icon: Optional[str], class_name: str, args
) -> None:
    filename = args.base_path + "/" + args.generate_code_path + "/" + class_name
    with open(filename + ".rc", "w") as file:
        file.write('#include "{}.h"\n'.format(class_name))
        if icon:
            file.write("IDI_APP_ICON ICON {}\n".format(args.base_path + "/" + icon))
        for k, v in binaries.items():
            file.write("BIN_{} RCDATA {}\n".format(k, args.base_path + "/" + v))

    with open(filename + ".h", "w") as file:
        file.write(note)
        file.write("#pragma once\n#ifdef RC_INVOKED\n")
        i = 1000
        for k, v in binaries.items():
            file.write("#define BIN_{} {}\n".format(k, i))
            i += 1
        file.write("#endif\n\n")

        file.write(
            "class {} {{\npublic:\nenum class Binaries : int {{\n".format(class_name)
        )
        i = 1000
        for k, v in binaries.items():
            file.write("\t{} = {},\n".format(k, i))
            i += 1
        file.write("};\n};")


def write_elf_object(binaries: Dict[str, str], class_name: str, args) -> None:
    filename = args.base_path + "/" + args.generate_code_path + "/" + class_name
    with open(filename + ".h", "w") as file:
        file.write(note)
        file.write("#pragma once\n\n")
        file.write(
            "class {} {{\npublic:\nenum class Binaries : int {{\n".format(class_name)
        )
        i = 0
        for k, v in binaries.items():
            file.write("\t{} = {},\n".format(k, i))
            i += 1
        file.write(
            f"}};\nstruct ResInfo {{\n\tconst char* start;\n\tunsigned long size;\n}};\nstatic ResInfo syms[{len(binaries)}];\n}};"
        )

    with open(filename + ".cpp", "w") as file:
        file.write('#include "{}.h"\n\n'.format(class_name))

        q: queue.Queue = queue.Queue()
        for k, v in binaries.items():
            name = sanitize_filename(v)
            file.write(
                "extern char _binary_{0}_start[];\nextern char _binary_{0}_end[];\n".format(
                    name
                )
            )
            q.put(name)

        file.write(
            "\n{0}::ResInfo {0}::syms[{1}] {{\n".format(class_name, len(binaries))
        )
        while not q.empty():
            name = q.get()
            file.write(
                "\t{{ _binary_{0}_start, (unsigned long)(_binary_{0}_end - _binary_{0}_start) }},\n".format(
                    name
                )
            )
        file.write("};\n")

    cmd_parts = ["ld", "-r", "-b", "binary"]
    for _, v in binaries.items():
        cmd_parts.append(v)
    output_file = os.path.join(
        args.base_path, args.generate_code_path, f"{class_name}.o"
    )
    cmd_parts.append("-o")
    cmd_parts.append(output_file)
    cmd = " ".join(cmd_parts)

    result = subprocess.run(
        cmd, shell=True, check=True, stderr=subprocess.PIPE, stdout=subprocess.PIPE
    )
    if result.returncode != 0:
        raise Exception(f"ld returned with non-zero value: {result.returncode}")


def write_mach_o_object(binaries: Dict[str, str], class_name: str, args) -> None:
    filename = args.base_path + "/" + args.generate_code_path + "/" + class_name
    q: queue.Queue = queue.Queue()
    with open(filename + ".h", "w") as file:
        file.write(note)
        file.write("#pragma once\n\n")
        file.write("class {} {{\npublic:\nenum class Binaries {{\n".format(class_name))
        i = 0
        for k, v in binaries.items():
            file.write("\t{} = {},\n".format(k, i))
            i += 1
        file.write(
            "}};\nstatic const char *syms[{0}];\n}};\nconst char *{1}::syms[{0}] {{\n".format(
                len(binaries), class_name
            )
        )
        for k, v in binaries.items():
            name = sanitize_filename(get_last_16_chars(v))
            q.put(name)
            file.write(f'\t"{name}",\n')
        file.write("};")

    with open(filename + ".link_options", "w") as file:
        first = True
        for k, v in binaries.items():
            if first:
                first = False
                file.write("{},{}".format(q.get(), args.base_path + "/" + v))
            else:
                file.write(";{},{}".format(q.get(), args.base_path + "/" + v))


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--base_path", type=str, required=True, help="Base path")
    parser.add_argument(
        "--resource_file_path",
        type=str,
        required=True,
        help="Path to the resource file",
    )
    parser.add_argument(
        "--generate_code_path", type=str, required=True, help="Path to generate code"
    )
    parser.add_argument(
        "--method_name", type=str, required=True, help="Method name(rc|elf|mach-o)"
    )
    args = parser.parse_args()

    class_name = Path(args.resource_file_path).stem
    binaries, icon = parse_file(args.resource_file_path)
    if args.method_name == "rc":
        write_windows_rc(binaries, icon, class_name, args)
    elif args.method_name == "elf":
        write_elf_object(binaries, class_name, args)
    elif args.method_name == "mach-o":
        write_mach_o_object(binaries, class_name, args)
    else:
        raise NotImplementedError


if __name__ == "__main__":
    main()