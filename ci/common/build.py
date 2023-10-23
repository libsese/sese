import os

from ci.common.config import BuildType


def build_config_all_with(build_dir: str):
    build_config_with(build_dir, 'all')


def build_config_with(build_dir: str, target: str):
    os.system('cmake --build {} --target {} -- -j4'.format(build_dir, target))


def build_ninja_config_all_with(build_dir: str, build_type: BuildType):
    build_ninja_config_with(build_dir, 'all', build_type)


def build_ninja_config_with(build_dir: str, target: str, build_type: BuildType):
    os.system(
        'cmake \
        --build {} \
        --target {} \
        --config {} \
        -j 8'
        .format(build_dir, target, BuildType.to_string(build_type)))
