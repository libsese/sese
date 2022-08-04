#include "sese/config/ConfigUtil.h"
#include "sese/record/LogHelper.h"

using sese::ConfigUtil;
using sese::ConfigFile;
using sese::record::LogHelper;

LogHelper helper("fCONFIG_UTIL"); // NOLINT

int main() {
    auto file = ConfigUtil::readFrom(PROJECT_PATH"/test/TestConfigUtil/data.ini");

    auto defaultSection = file->getDefaultSection();
    auto defaultName = defaultSection->getValueByKey("name", "unknown");
    helper.info( "[default] name=%s", defaultName.c_str());;

    auto serverSection = file->getSectionByName("server");
    auto serverAddress = serverSection->getValueByKey("address", "192.168.1.1");
    auto serverPort = serverSection->getValueByKey("port", "8080");
    helper.info("[server] address=%s", serverAddress.c_str());
    helper.info("[server] port=%s", serverPort.c_str());

    auto clientSection = file->getSectionByName("client");
    if(clientSection == nullptr){
        helper.warn("No section named \"client\"");
    }

    ConfigUtil::write2(file, "data.ini");

    return 0;
}