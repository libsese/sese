#include "config/ConfigUtil.h"
#include "Util.h"

#define FILTER_TEST_CONFIG_UTIL "fCONFIG_UTIL"

using sese::ConfigFile;
using sese::ConfigUtil;

int main() {
    auto file = ConfigUtil::readFrom(PROJECT_PATH"/test/TestConfigUtil/data.ini");

    auto defaultSection = file->getDefaultSection();
    auto defaultName = defaultSection->getValueByKey("name", "unknown");
    ROOT_INFO(FILTER_TEST_CONFIG_UTIL, "[default] name=%s", defaultName.c_str());

    auto serverSection = file->getSectionByName("server");
    auto serverAddress = serverSection->getValueByKey("address", "192.168.1.1");
    auto serverPort = serverSection->getValueByKey("port", "8080");
    ROOT_INFO(FILTER_TEST_CONFIG_UTIL, "[server] address=%s", serverAddress.c_str());
    ROOT_INFO(FILTER_TEST_CONFIG_UTIL, "[server] port=%s", serverPort.c_str());

    auto clientSection = file->getSectionByName("client");
    if(clientSection == nullptr){
        ROOT_WARN(FILTER_TEST_CONFIG_UTIL, "No section named \"client\"");
    }

    ConfigUtil::write2(file, PROJECT_PATH"/output/data.ini");

    return 0;
}