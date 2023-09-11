import os.path

vcpkg_windows_path = [
    'C:/vcpkg/',
    'C:/src/vcpkg/',
    'D:/vcpkg/',
    'E:/vcpkg/'
]

vcpkg_unix_path = [
    '/usr/local/vcpkg/',
    '/src/vcpkg/'
]

vcpkg_relative_path = 'scripts/buildsystems/vcpkg.cmake'


def find_vcpkg():
    paths = list()

    if os.name == 'nt':
        paths = vcpkg_windows_path
    else:
        paths = vcpkg_unix_path

    for path in paths:
        if os.path.exists(path + vcpkg_relative_path):
            return path + vcpkg_relative_path
    return ''


def main():
    path_to_vcpkg = find_vcpkg()
    if path_to_vcpkg == '':
        print('can not found the vcpkg.cmake')
        exit(-1)

    os.system('cmake -DCMAKE_TOOLCHAIN_FILE={} -DSESE_BUILD_TEST=ON -Bbuild'.format(path_to_vcpkg))


if __name__ == '__main__':
    main()
