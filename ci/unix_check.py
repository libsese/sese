import ci.common.check as ch

assert ch.check_vcpkg()
assert ch.check_cmake()
assert ch.check_ctest()
assert ch.check_gcov()
