import enum
import os

import ci.common.vcpkg as vcpkg


class BuildType(enum.Enum):
    DEBUG = 0,
    RELEASE = 1

    @staticmethod
    def to_string(t):
        if t == BuildType.DEBUG:
            return "Debug"
        elif t == BuildType.RELEASE:
            return "Release"


def config_with(build_type: BuildType, build_dir: str):
    path_to_vcpkg = vcpkg.find_vcpkg()
    if path_to_vcpkg == '':
        print('can not found the vcpkg.cmake')
        exit(-1)

    os.system(
        'cmake \
        -DCMAKE_TOOLCHAIN_FILE={} \
        -DCMAKE_BUILD_TYPE={} \
        -DSESE_BUILD_TEST=ON \
        -B{}'
        .format(path_to_vcpkg, BuildType.to_string(build_type), build_dir))


def config_with_ninja(build_type: BuildType, build_dir: str):
    path_to_vcpkg = vcpkg.find_vcpkg()
    if path_to_vcpkg == '':
        print('can not found the vcpkg.cmake')
        exit(-1)

    os.system(
        'cmake \
        -G Ninja \
        -DCMAKE_TOOLCHAIN_FILE={} \
        -DCMAKE_BUILD_TYPE={} \
        -DSESE_BUILD_TEST=ON \
        -B{}'
        .format(path_to_vcpkg, BuildType.to_string(build_type), build_dir))


def config_with_coverage(build_type: BuildType, build_dir: str):
    path_to_vcpkg = vcpkg.find_vcpkg()
    if path_to_vcpkg == '':
        print('can not found the vcpkg.cmake')
        exit(-1)

    os.system(
        'cmake \
        -DCMAKE_TOOLCHAIN_FILE={} \
        -DCMAKE_BUILD_TYPE={} \
        -DCMAKE_CXX_FLAGS="--coverage -fprofile-update=atomic" \
        -DSESE_BUILD_TEST=ON \
        -B{}'
        .format(path_to_vcpkg, BuildType.to_string(build_type), build_dir))
