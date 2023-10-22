import os
from ci.config import BuildType

def build_with_current_config():
    os.system('cmake --build build -- -j4')


def build_with_current_config(target: str):
    os.system('cmake --build build --target {} -- -j4'.format(target))


def build_with_ninja_currnet_config(type):
    os.system('cmake --build build --config {} -j 8'.format(BuildType.to_string(type)))


def build_with_ninja_currnet_config(target: str, type):
    os.system('cmake --build build --target {} --config {} -j 8'.format(target, BuildType.to_string(type)))


