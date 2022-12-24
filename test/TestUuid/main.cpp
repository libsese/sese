#include <sese/UuidBuilder.h>
#include <sese/record/LogHelper.h>

static const uint8_t ID = 114;

int main() {
    sese::record::LogHelper log("uuid");
    sese::UuidBuilder builder;
    uuid::Uuid id(ID, 0, 0);

    for (auto i = 0; i < 10; ++i) {
        // 模拟负载
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        if (builder.generate(id)) {
            log.debug("%d : ok -> %lld", i, id.toNumber());
        } else {
            log.debug("%d : no", i);
        }
    }
}