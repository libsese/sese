#include <sese/Init.h>
#include <sese/Log.h>

int main(int argc, char **argv) {
    sese::initCore(argc, argv);
    SESE_INFO("hello sese");
    return 0;
}