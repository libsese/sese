import os
import subprocess
import requests
import hashlib

from pygments.lexers.robotframework import Variable

# 获取最新的 commit hash
result = subprocess.run(['git', '-C', '.', '--no-pager', 'log', '-1', '--pretty=format:%H'], stdout=subprocess.PIPE)
hash_code = result.stdout.decode('utf-8').strip()

# 构建文件名
file_name = f"{hash_code}.tar.gz"

# 下载文件并计算 sha512 hash
url = f"https://github.com/libsese/sese/archive/{file_name}"
response = requests.get(url)

sha512 = hashlib.sha512()
sha512.update(response.content)
archive_hash_code = sha512.hexdigest()

# 输出结果
print(f"commit hash {hash_code}")
print(f"archive hash {archive_hash_code}")

variables = {
    'REF': hash_code,
    'SHA512': archive_hash_code
}

current_dir = os.path.dirname(__file__)
with open(os.path.join(current_dir, '../package/ports/sese/portfile.cmake.in'), 'r') as infile:
    template = infile.read()

for key, value in variables.items():
    template = template.replace(f'@@{key}@@', value)

with open(os.path.join(current_dir, '../package/ports/sese/portfile.cmake'), 'w') as outfile:
    outfile.write(template)

print('ok')