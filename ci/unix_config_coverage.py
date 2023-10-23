import ci.common.config as config
from ci.common.config import BuildType

config.config_with_coverage(BuildType.DEBUG, 'build-coverage')
