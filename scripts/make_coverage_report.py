import os
import sys


def main():
    if len(sys.argv) != 2:
        print('参数不足')
        sys.exit(-1)

    build_dir = sys.argv[1]

    if not os.path.exists('{}/{}'.format(os.getcwd(), build_dir)):
        print('指定路径不存在')
        sys.exit(-1)

    cov_dir = '{}/coverage'.format(os.getcwd());
    print('checking directory... {}'.format(cov_dir))
    if not os.path.exists(cov_dir):
        print('mkdir {}'.format(cov_dir))
        os.mkdir(cov_dir)

    print('process is running... gcov')
    sys.exit(os.system('gcovr -b -r {} --html --html-details -o {}/index.html'.format(build_dir, cov_dir)))


if __name__ == '__main__':
    main()
