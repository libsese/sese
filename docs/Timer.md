# Timer

内建 sese::Timer 的 tick 为 1 秒，且将直接在 Timer 线程上执行任务 callback 函数。

若需调度耗时任务，请考虑重写 **sese::Timer::execute(const std::function<void()> &) : void** 方法。

例如：

```cpp
// 核心代码 - 提交任务至线程池
void MyTimer::execute(const std::function<void()> &callback) {
    threadPool.postTask(callback);
}
```



