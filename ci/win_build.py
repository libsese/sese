import ci.common.build as build
from ci.common.build import BuildType

build.build_ninja_config_all_with('build', BuildType.RELEASE)
