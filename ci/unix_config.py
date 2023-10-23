import ci.common.config as config
from ci.common.config import BuildType

config.config_with(BuildType.RELEASE, 'build')
