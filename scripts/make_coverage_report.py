import os
import sys


def main():
    if len(sys.argv) != 2:
        print('参数不足')
        sys.exit(-1)

    build_dir = sys.argv[1]

    if not os.path.exists(build_dir):
        print('指定路径不存在')
        sys.exit(-1)

    if not os.path.exists(os.getcwd() + '/coverage'):
        os.mkdir('coverage')

    sys.exit(os.system('gcovr -b -r {} --html --html-details -o coverage/index.html'.format(build_dir)))


if __name__ == '__main__':
    main()
