#include "Util.h"
#include "thread/ThreadPool.h"
#include <cmath>

#define FILTER_THREAD_POOL "fTHREAD_POOL"

using namespace sese;

class MyTask : public Task {
public:
    int32_t value = 0;

    explicit MyTask(int32_t value) : value(value) {}

    [[nodiscard]] std::function<void()> getFunction() const noexcept override {
        return [v = value] {
            auto rt = std::tgamma(v);
            ROOT_INFO(FILTER_THREAD_POOL, "rt: %f", rt)
        };
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