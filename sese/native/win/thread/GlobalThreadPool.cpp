#include <sese/thread/GlobalThreadPool.h>

DWORD WINAPI sese::GlobalThreadPool::taskRunner1(LPVOID lp_param) {
    auto task1 = static_cast<Task1 *>(lp_param);
    task1->function();
    delete task1;
    return 0;
}

void sese::GlobalThreadPool::postTask(const std::function<void()> &func) {
    auto task1 = new Task1;
    task1->function = func;
    QueueUserWorkItem(taskRunner1, task1, WT_EXECUTEDEFAULT);
}
