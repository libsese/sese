#include "config/UniReader.h"

#define FILTER_TEST_UNI_READER "fUNI_READER"

using sese::UniReader;

int main() {
    auto reader = std::make_shared<UniReader>(PROJECT_PATH "/test/TestUniReader/data.txt");
    while(true){
        auto line = reader->readLine();
        if (line.empty()) {
            break;
        } else {
            ROOT_INFO(FILTER_TEST_UNI_READER, "%s", line.c_str())
        }
    }
    return 0;
}
