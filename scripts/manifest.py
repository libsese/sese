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
import re


def parse_version(version):
    pattern = r'(\d+)\.(\d+)\.(\d+)(?:-(\S+))?'
    match = re.match(pattern, version)
    if match:
        major = int(match.group(1))
        minor = int(match.group(2))
        patch = int(match.group(3))
        pre_release = match.group(4)
        return {
            'major': major,
            'minor': minor,
            'patch': patch,
            'pre_release': pre_release
        }
    return None


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--manifest_file_path', type=str, required=True, help="Path to the manifest file")
    parser.add_argument('--output_file_path', type=str, required=True, help="Path to output file")
    parser.add_argument('--filename', type=str, required=True, help="Original file name")
    args = parser.parse_args()

    with open(args.manifest_file_path, 'r') as file:
        config = json.load(file)

    versions = parse_version(config['version'])

    content = f"""1 VERSIONINFO

FILEVERSION {versions['major']},{versions['minor']},{versions['patch']},0
PRODUCTVERSION {versions['major']},{versions['minor']},{versions['patch']},0
FILEFLAGSMASK 0x3FL
#ifdef _DEBUG
FILEFLAGS 0x1L
#else
FILEFLAGS 0x0L
#endif
FILEOS 0x4L
FILETYPE 0x0L
FILESUBTYPE 0x0L
BEGIN
    BLOCK"StringFileInfo"
    BEGIN
        BLOCK"040904E4"
        BEGIN
            VALUE"CompanyName", "{config['author']}"
            VALUE"ProductName", "{config['productName']}"
            VALUE"InternalName", "{config['internalName']}"
            VALUE"ProductVersion", "{config['version']}"
            VALUE"FileDescription", "{config['description']}"
            VALUE"FileVersion", {versions['major']},{versions['minor']},{versions['patch']},0
            VALUE"LegalCopyright", "{config['copyright']}"
            VALUE"OriginalFilename", "{args.filename if args.filename != "filename-NOTFOUND" else "UNDEFINED"}"
        END
    END
    BLOCK"VarFileInfo"
    BEGIN
        VALUE"Translation", 0x409, 1200
    END
END
    """

    with open(args.output_file_path, 'w') as file:
        file.write(content)


if __name__ == "__main__":
    main()
