import os
import sys


def main():
    cov_dir = '{}/coverage'.format(os.getcwd())
    print('checking directory... {}'.format(cov_dir))
    if not os.path.exists(cov_dir):
        print('mkdir {}'.format(cov_dir))
        os.mkdir(cov_dir)

    print('process is running... gcov')
    sys.exit(os.system('gcovr'))


if __name__ == '__main__':
    main()
