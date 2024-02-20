/**
 * @file Exception.h
 * @brief 异常类
 * @author kaoru
 * @date 2023年9月26日
 */

#pragma once

#include <sese/Config.h>
#include <sese/io/OutputStream.h>
#include <sese/record/Logger.h>
#include <sese/system/StackInfo.h>

#include <exception>

#ifdef SESE_PLATFORM_WINDOWS
#pragma warning(disable : 4275)
#endif

namespace sese {

#if defined(SESE_PLATFORM_WINDOWS)
using NativeException = std::exception;
#else
/// Unix 异常包装类
class UnixException : public std::exception {
public:
    explicit UnixException(const char *message) : std::exception(), msg(message) {
    }

    [[nodiscard]] const char *what() const noexcept override {
        return msg;
    }

private:
    const char *msg{};
};

using NativeException = UnixException;
#endif

/// 异常类
class API Exception : public NativeException {
public:
    explicit Exception(const char *message);

    ~Exception() override;

    /// 向标准输出打印堆栈跟踪
    void printStacktrace();

    /// 向日志器打印堆栈跟踪
    /// \param logger 日志器
    void printStacktrace(sese::record::Logger *logger);

    /// 向输出流打印堆栈跟踪
    /// \param output 输出流
    void printStacktrace(sese::io::OutputStream *output);

    static int getSkipOffset();

protected:

    virtual std::string buildStacktrace();

    sese::system::StackInfo *stackInfo{};
};
} // namespace sese