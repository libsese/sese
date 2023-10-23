import os.path

vcpkg_windows_path = [
    'C:/vcpkg',
    'C:/src/vcpkg',
    'D:/vcpkg',
    'E:/vcpkg'
]

vcpkg_unix_path = [
    '/usr/local/share/vcpkg',
    '/usr/local/vcpkg',
    '/src/vcpkg'
]

vcpkg_relative_path = '/scripts/buildsystems/vcpkg.cmake'


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
