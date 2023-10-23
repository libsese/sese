import ci.common.config as config
from ci.common.config import BuildType

config.config_with_ninja(BuildType.RELEASE, 'build')
