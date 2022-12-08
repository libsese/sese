#include <sese/system/SharedMemory.h>
#include <sese/record/LogHelper.h>
#include <sese/Test.h>

#ifdef WIN32
#include <sese/thread/Thread.h>
sese::record::LogHelper helper("fTestSharedMemory");
void func() {
    auto mem = sese::SharedMemory::use("TestSharedMemory");
    assert(helper, mem != nullptr, -2);
    helper.debug("read mem: %s", mem->getBuffer());
}

int main() {
    auto mem = sese::SharedMemory::create("TestSharedMemory", 1024);
    assert(helper, mem != nullptr, -1);
    strcpy((char *) mem->getBuffer(), "Hello");
    helper.debug("write succeeded");

    auto th = std::make_shared<sese::Thread>(func);
    th->start();
    th->join();
    return 0;
}
#else
#include <unistd.h>
int main(int argc, char **argv) {
    sese::record::LogHelper log("fTestSharedMemory");

    auto id = fork();
    assert(log, id != -1, -1);
    if (id == 0) {
        auto mem = sese::SharedMemory::use("TestSharedMemory");
        assert(log, mem != nullptr, -2);
        log.debug("read mem: %s", mem->getBuffer());
        exit(0);
    } else {
        auto mem = sese::SharedMemory::create("TestSharedMemory", 1024);
        assert(log, mem != nullptr, -3);
        strcpy((char *) mem->getBuffer(), "Hello");
        log.debug("write succeeded");
        sleep(3);
        exit(0);
    }
}
#endif