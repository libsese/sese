import os
from ci.config import BuildType


def test_with(type):
    os.system('ctest -C {} --extra-verbose'.format(BuildType.to_string(type)))


def make_coverage_report():
    cov_dir = '{}/coverage'.format(os.getcwd())
    print('checking directory... {}'.format(cov_dir))
    if not os.path.exists(cov_dir):
        print('mkdir {}'.format(cov_dir))
        os.mkdir(cov_dir)

    print('process is running... gcovr')
    os.system('gcovr')