import ci.common.test as test
from ci.common.config import BuildType

test.test_with('cmake-build-debug-visual-studio', BuildType.RELEASE)
