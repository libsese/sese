#include <sese/system/SharedMemory.h>
#include <sese/record/LogHelper.h>
#include <sese/util/Random.h>

#include <chrono>
#include <memory>
#include <thread>

using namespace std::chrono_literals;

#include <sese/util/Initializer.h>

int main() {
    sese::Initializer::getInitializer();
    sese::record::LogHelper helper;
    auto mem = sese::system::SharedMemory::create("SharedMemoryForSese", sizeof(int64_t));
    if (mem == nullptr) {
        helper.error("failed to create shared memory: errno %d", errno);
        return -1;
    } else {
        helper.info("create success");
    }

    auto random = (int64_t) sese::Random::next();
    helper.info("select random number: %lld", random);
    auto buffer = mem->getBuffer();
    auto pInt = (int64_t *) buffer;
    *pInt = random;

    while (true) {
        if (*pInt == random + 1) {
            break;
        } else {
            std::this_thread::sleep_for(500ms);
        }
    }

    helper.info("recv change number: %lld", *pInt);

    return 0;
}