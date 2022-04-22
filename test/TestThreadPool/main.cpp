#include "sese/Util.h"
#include "sese/record/LogHelper.h"
#include "sese/thread/ThreadPool.h"
#include <cmath>

using namespace sese;

LogHelper helper("fTHREAD_POOL"); //NOLINT

class MyTask : public Task {
public:
    int32_t value = 0;
    double_t rt = 0;

    explicit MyTask(int32_t value) : value(value) {}

    void content() noexcept override {
        rt = std::tgamma(value);
        helper.info("rt: %f", rt);
    }
};

int main() {
    std::vector<Task::Ptr> tasks(80);
    for (int32_t i = 20; i < 100; i++) {
        tasks.emplace_back(std::make_shared<MyTask>(i));
    }

    ThreadPool pool("threadPool", 4);
    pool.postTask(tasks);

    sese::sleep(2);
    pool.shutdown();
    return 0;
}