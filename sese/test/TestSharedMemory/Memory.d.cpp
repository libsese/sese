#include <sese/system/SharedMemory.h>
#include <sese/record/LogHelper.h>

#include <chrono>
#include <memory>
#include <thread>
#include <random>

using namespace std::chrono_literals;

#include <sese/util/Initializer.h>

int main(int argc, char **argv) {
    sese::initCore(argc, argv);
    sese::record::LogHelper helper;
    auto mem = sese::system::SharedMemory::create("SharedMemoryForSese", sizeof(int64_t));
    if (mem == nullptr) {
        helper.error("failed to create shared memory: errno %d", errno);
        return -1;
    } else {
        helper.info("create success");
    }

    auto random = static_cast<int64_t>(std::random_device()());
    helper.info("select random number: %lld", random);
    auto buffer = mem->getBuffer();
    auto p_int = static_cast<int64_t *>(buffer);
    *p_int = random;

    while (true) {
        if (*p_int == random + 1) {
            break;
        } else {
            std::this_thread::sleep_for(500ms);
        }
    }

    helper.info("recv change number: %lld", *p_int);

    return 0;
}