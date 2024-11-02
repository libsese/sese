# 错误处理

目前框架存在三种细分类别的错误处理方式，以下做详细介绍和演示。

## sese::Result\<T\>

这是框架推荐使用的错误处理方式，它支持返回正确结果或者错误码。

```c++
#include <sese/util/Result.h>
#include <sese/Init.h>

// 设计一个使用 Resutl<T> 的 API
sese::Result<long long> to_int(const std::string &str) {
    char *end;
    auto num = std::strtol(str.c_str(), &end, 10);
    if (end) {
        return {1, "Failed to parse string to num"};
    }
    return num;
}

// 处理 Result<T> 返回值
int main(int argc, char **argv) {
    sese::initCore(argc, argv);
    auto result = to_init("114514homo");
    if (result) {
        SESE_ERROR("{}", result.err().message());
        return result.err().value();
    }
    auto num = result.get();
    SESE_INFO("{}", num);
    return 0;
}
```

## sese::Exception

这是 `C++` 中标准的错误处理方式，通过 try/catch 的方式捕获异常并且处理异常。
**虽然框架API不会抛出异常**，但在此基础上，框架提供了功能更多的 `sese::Expection`。

```c++
#include <sese/Init.h>
#include <sese/util/Exception.h>

int main(int argc, char **argv) {
    sese::initCore(argc, argv);
    try {
        throw sese::Expection(std::string("Hello ") + std::to_string(1919810));
    } catch (sese::Expection &e) {
        e.printStacktrace();
    }
    return 0;
}
```

由框架提供的异常支持携带调用栈信息和动态构建的字符串。

## Classic

这是 `C` 中常见的错误处理方式以直接返回整形或者指针判空作为判断错误的依据。
在框架设计之初，保留了相当一部分这种设计。
