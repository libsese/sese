# Error Handling

Currently, the framework has three subcategories of error handling methods. Here's a detailed introduction and demonstration.

## sese::Result\<T\>

This is the recommended error handling method for the framework. It supports returning either the correct result or an error code.

```c++
#include <sese/util/Result.h>
#include <sese/Init.h>

// Design an API using Result<T>
sese::Result<long long> to_int(const std::string &str) {
    char *end;
    auto num = std::strtol(str.c_str(), &end, 10);
    if (end) {
        return {1, "Failed to parse string to num"};
    }
    return num;
}

// Handle the Result<T> return value
int main(int argc, char **argv) {
    sese::initCore(argc, argv);
    auto result = to_int("114514homo");
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

This is the standard error handling method in `C++`, which captures and handles exceptions through try/catch.
**Although the framework API does not throw exceptions**, it provides a more functional `sese::Exception`.

```c++
#include <sese/Init.h>
#include <sese/util/Exception.h>

int main(int argc, char **argv) {
    sese::initCore(argc, argv);
    try {
        throw sese::Exception(std::string("Hello ") + std::to_string(1919810));
    } catch (sese::Exception &e) {
        e.printStacktrace();
    }
    return 0;
}
```

The exceptions provided by the framework support carrying call stack information and dynamically constructed strings.

## Classic

This is a common error handling method in `C`, which uses direct return of integers or null pointer checks as error judgment criteria.
A significant portion of this design was retained in the initial design of the framework.
