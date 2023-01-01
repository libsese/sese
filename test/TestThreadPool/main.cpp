#include "sese/util/Util.h"
#include "sese/record/LogHelper.h"
#include "sese/thread/ThreadPool.h"
#include <cmath>

using namespace sese;

record::LogHelper helper("fTHREAD_POOL"); //NOLINT

int main() {
    auto task = [&helper = helper](int value){
        helper.info("rt: %f", std::tgamma(value));
    };

    std::vector<std::function<void ()> > tasks(80);
    for (int32_t i = 20; i < 100; i++) {
        std::function<void ()> t = [task, i] { return task(i); };
        tasks.emplace_back(t);
    }

    ThreadPool pool("threadPool", 4);
    pool.postTask(tasks);

    sese::sleep(2);
    pool.shutdown();
    return 0;
}