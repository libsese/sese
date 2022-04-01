#include "system/Environment.h"
#include "Util.h"

#define FILTER_TEST_ENV "fENV"

using sese::Environment;

int main() {
    ROOT_INFO(FILTER_TEST_ENV,
              "Your operating system: %s",
              Environment::getOperateSystemType())

    ROOT_INFO(FILTER_TEST_ENV,
              "libsese version: %d.%d",
              Environment::getMajorVersion(),
              Environment::getMinorVersion())

    ROOT_INFO(FILTER_TEST_ENV,
              "libsese repo on the %s branch: %s",
              Environment::getRepoBranch(),
              Environment::getRepoHash())

    ROOT_INFO(FILTER_TEST_ENV,
              "libsese build on %s %s - %s",
              Environment::getBuildDate(),
              Environment::getBuildTime(),
              Environment::getBuildDateTime())

    ROOT_INFO(FILTER_TEST_ENV,
              "system endian: %s",
              Environment::isLittleEndian() ? "little endian" : "big endian")

    return 0;
}