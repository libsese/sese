#include "system/Environment.h"
#include "Util.h"

using sese::Environment;

int main() {
    ROOT_INFO("Your operating system: %s",
              Environment::getOperateSystemType())

    ROOT_INFO("libsese version: %d.%d",
              Environment::getMajorVersion(),
              Environment::getMinorVersion())

    ROOT_INFO("libsese repo on the %s branch : %s",
              Environment::getRepoBranch(),
              Environment::getRepoHash())

    ROOT_INFO("libsese build on %s %s - %s",
              Environment::getBuildDate(),
              Environment::getBuildTime(),
              Environment::getBuildTimestamp())

    return 0;
}