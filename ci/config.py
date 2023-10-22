import enum
import ci.vcpkg as vcpkg
import os


class BuildType(enum.Enum):
    DEBUG = 0,
    RELEASE = 1

    def to_string(type):
        if type == BuildType.DEBUG:
            return "Debug"
        elif type == BuildType.RELEASE:
            return "Release"


def config_with(type):
    path_to_vcpkg = vcpkg.find_vcpkg()
    if path_to_vcpkg == '':
        print('can not found the vcpkg.cmake')
        exit(-1)

    os.system('''cmake 
              -DCMAKE_TOOLCHAIN_FILE={} 
              -DCMAKE_BUILD_TYPE={} 
              -DSESE_BUILD_TEST=ON -Bbuild'''
              .format(path_to_vcpkg, BuildType.to_string(type))
              )


def config_with_ninja(type):
    path_to_vcpkg = vcpkg.find_vcpkg()
    if path_to_vcpkg == '':
        print('can not found the vcpkg.cmake')
        exit(-1)

    os.system('''cmake 
              -G Ninja 
              -DCMAKE_TOOLCHAIN_FILE={} 
              -DCMAKE_BUILD_TYPE={} 
              -DSESE_BUILD_TEST=ON 
              -Bbuild'''
              .format(path_to_vcpkg, BuildType.to_string(type))
              )


def config_with_coverage(type):
    path_to_vcpkg = vcpkg.find_vcpkg()
    if path_to_vcpkg == '':
        print('can not found the vcpkg.cmake')
        exit(-1)

    os.system('''cmake 
              -DCMAKE_TOOLCHAIN_FILE={} 
              -DCMAKE_BUILD_TYPE={} 
              -DCMAKE_CXX_FLAGS="--coverage -fprofile-update=atomic" 
              -DSESE_BUILD_TEST=ON -Bbuild'''
              .format(path_to_vcpkg, BuildType.to_string(type))
              )
    
