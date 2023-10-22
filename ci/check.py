import os
from vcpkg import find_vcpkg

def check_cmake():
    return os.system("cmake --version") == 0


def check_ctest():
    return os.system("ctest --version") == 0


def check_gcov():
    return os.system("gcov --version") == 0

def check_vcpkg():
    return find_vcpkg() != ''