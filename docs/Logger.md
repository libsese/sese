# Logger

目前提供两种日志输出手段

- 使用 Util.h 中的宏
- LogHelper

## 对比

| LogHelper                             | ~~Marco~~                   |
| ------------------------------------- | --------------------------- |
| debug(const char *format, ...) : void | ~~ROOT_DEBUG(FILTER, ...)~~ |
| info(const char *format, ...) : void  | ~~ROOT_INFO(FILTER, ...)~~  |
| warn(const char *format, ...) : void  | ~~ROOT_WARN(FILTER, ...)~~  |
| error(const char *format, ...) : void | ~~ROOT_ERROR(FILTER, ...)~~ |

我的项目更适用哪种方式来输出日志？

- LogHelper 通常不包含有效的文件于行号信息，相较于宏减少 getLogger() 次数，在大量输出情况下性能更好一些。
- Marco 包含完整的日志事件信息，但需要手动指定过滤器。

过滤器是什么？

- 过滤器是一个由用户手动指定的标识符，该标识符可用于表示不同的模块或者是功能。