import subprocess
import requests
import hashlib

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