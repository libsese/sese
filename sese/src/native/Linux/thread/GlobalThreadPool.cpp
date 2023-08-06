#include <sese/thread/GlobalThreadPool.h>

void sese::GlobalThreadPool::postTask(const std::function<void()> &func) {
    auto pool = globalThreadPool.getInstance();
    pool->postTask(func);
}