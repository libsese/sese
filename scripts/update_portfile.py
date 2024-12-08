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

import hashlib
import os
import subprocess

import requests

# Get latest commit hash
result = subprocess.run(
    ["git", "-C", ".", "--no-pager", "log", "-1", "--pretty=format:%H"],
    stdout=subprocess.PIPE,
)
hash_code = result.stdout.decode("utf-8").strip()

# Build the file name
file_name = f"{hash_code}.tar.gz"

# Download the file and calculate the sha512 hash
url = f"https://github.com/libsese/sese/archive/{file_name}"
response = requests.get(url)

sha512 = hashlib.sha512()
sha512.update(response.content)
archive_hash_code = sha512.hexdigest()

# print results
print(f"commit hash {hash_code}")
print(f"archive hash {archive_hash_code}")

variables = {"REF": hash_code, "SHA512": archive_hash_code}

current_dir = os.path.dirname(__file__)
with open(
    os.path.join(current_dir, "../package/ports/sese/portfile.cmake.in"), "r"
) as infile:
    template = infile.read()

for key, value in variables.items():
    template = template.replace(f"@@{key}@@", value)

with open(
    os.path.join(current_dir, "../package/ports/sese/portfile.cmake"), "w"
) as outfile:
    outfile.write(template)

print("ok")
