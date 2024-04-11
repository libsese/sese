import os


def check_dir(directory: str):
    target = '{}/{}'.format(os.getcwd(), directory)
    if not os.path.exists(target):
        print('mkdir {}'.format(target))
        os.makedirs(target)


def main():
    check_dir('build/coverage/html')

    print('process is running... gcovr')
    os.system('gcovr --config gcovr-html.cfg')
    print('\n================================\n')
    os.system('gcovr --config gcovr-cobertura.cfg')


if __name__ == '__main__':
    main()
