import os
import sys
import shutil


# 测试当前目录
def test_root_path(path: str):
    fileA = open(path + 'FileA.txt', 'wt')
    fileB = open(path + 'FileB.txt', 'a')
    fileA.write('Hello')
    fileB.close()
    fileA.close()
    shutil.copyfile(path + 'FileA.txt', path + 'FileC.txt')
    os.rename(path + 'FileB.txt', path + 'FileD.txt')


# 测试子目录
def test_sub_path(path: str):
    subdir = path + 'subdir/'
    os.makedirs(subdir)
    subA = open(subdir + 'subA.txt', 'wt')
    subB = open(subdir + 'subB.txt', 'a')
    subA.write('hello')
    subB.close()
    subA.close()


# 拷贝子目录文件至根目录
# copy $src/subA.txt to $dst/FileE.txt
def test_copy_cross_path(src: str, dst: str):
    shutil.copyfile(src + 'subA.txt', dst + 'FileE.txt')


# 清理
def cleanup(path: str):
    os.remove(path + 'FileA.txt')
    os.remove(path + 'FileC.txt')
    os.remove(path + 'FileD.txt')
    os.remove(path + 'FileE.txt')
    os.remove(path + 'subdir/' + 'subA.txt')
    os.remove(path + 'subdir/' + 'subB.txt')
    os.rmdir(path + 'subdir')


def main():
    # path = 'C:/temp/'
    # path = '/tmp/'
    path = sys.argv[1]
    if not os.path.exists(path):
        print('creating test dir')
        os.mkdir(path)
    print('changing files')
    test_root_path(path)
    test_sub_path(path)
    test_copy_cross_path(path + 'subdir/', path)
    cleanup(path)
    print('done')


if __name__ == '__main__':
    main()
