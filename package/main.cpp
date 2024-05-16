#include <sese/util/Initializer.h>
#include <sese/record/Marco.h>

int main(int argc, char **argv) {
    sese::initCore(argc, argv);
    SESE_INFO("hello sese");
    return 0;
}