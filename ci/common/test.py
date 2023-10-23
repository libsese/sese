import os

from ci.common.config import BuildType


def test_with(build_dir: str, build_type: BuildType):
    os.system('cd {} && ctest -C {} --extra-verbose'.format(build_dir, BuildType.to_string(build_type)))


def make_coverage_report():
    cov_dir = '{}/coverage'.format(os.getcwd())
    print('checking directory... {}'.format(cov_dir))
    if not os.path.exists(cov_dir):
        print('mkdir {}'.format(cov_dir))
        os.mkdir(cov_dir)

    print('process is running... gcovr')
    os.system('gcovr')
