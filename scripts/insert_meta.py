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

def read_meta_tag(meta_file):
    try:
        with open(meta_file, 'r', encoding='utf-8') as file:
            return file.read().strip()  # 返回 meta 标签内容
    except FileNotFoundError:
        raise FileNotFoundError(f"Error: Meta file '{meta_file}' not found.")

def insert_meta_tag(html_file, meta_tag):
    try:
        with open(html_file, 'r', encoding='utf-8') as file:
            content = file.readlines()
    except FileNotFoundError:
        raise FileNotFoundError(f"Error: HTML file '{html_file}' not found.")

    head_index = -1
    for i, line in enumerate(content):
        if "<head>" in line:
            head_index = i
            break

    if head_index == -1:
        raise ValueError(f"Error: No <head> tag found in '{html_file}'.")

    content.insert(head_index + 1, meta_tag + '\n')

    with open(html_file, 'w', encoding='utf-8') as file:
        file.writelines(content)

    print(f"Successfully inserted meta tag into '{html_file}'.")

def main():
    parser = argparse.ArgumentParser(description="Insert a meta tag into an HTML file.")
    parser.add_argument("--html_file", help="The target HTML file to modify.")
    parser.add_argument("--meta_file", help="The file containing the meta tag text.")

    args = parser.parse_args()

    try:
        meta_tag = read_meta_tag(args.meta_file)  # 读取 meta 标签内容
        insert_meta_tag(args.html_file, meta_tag)  # 插入 meta 标签
    except Exception as e:
        print(e)

if __name__ == "__main__":
    main()
