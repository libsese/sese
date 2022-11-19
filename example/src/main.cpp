#include <sese/record/LogHelper.h>

sese::LogHelper helper("example"); // NOLINT

int main() {
    helper.info("This message sent by sese::LogHelper!");
    return 0;
}