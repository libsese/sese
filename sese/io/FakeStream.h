/**
 * @file FakeStream.h
 * @author kaoru
 * @brief 伪装流模板
 * @version 0.1
 * @date 2023年9月13日
 */

#pragma once

#include "sese/Config.h"
#include "sese/io/Closeable.h"
#include "sese/io/PeekableStream.h"
#include "sese/io/Stream.h"

#include <istream>
#include <ostream>

namespace sese::io {

// 此处已经过单测，不存在分支和未测函数
// GCOVR_EXCL_START

/// \brief 伪装流
/// \tparam T 包装类型
template<typename T>
class FakeStream : public Stream {
public:
    explicit FakeStream(T *t) : t(t) {}

    int64_t read(void *buffer, size_t length) override { return t->read(buffer, length); }
    int64_t write(const void *buffer, size_t length) override { return t->write(buffer, length); };

protected:
    T *t;
};

/// \brief 可关闭的伪装流
/// \tparam T 包装类型
template<typename T>
class ClosableFakeStream : public FakeStream<T>, public sese::io::Closeable {
public:
    explicit ClosableFakeStream(T *t) : FakeStream<T>(t) {}
    void close() override { this->t->close(); }
};

/// @brief 标准库输入包装流
class StdInputStreamWrapper final
    : public InputStream,
      public PeekableStream {
public:
    explicit StdInputStreamWrapper(std::istream &stream);

    int64_t read(void *buffer, size_t length) override;

    /// 窥视一定字节数
    /// \warning 不推荐使用该函数，除非你很明白你在做什么
    /// \see sese::StdOutputStreamWrapper::write
    /// \param buffer 缓存
    /// \param length 缓存大小
    /// \return 实际读取大小
    int64_t peek(void *buffer, size_t length) override;

    /// 步进一定字节数
    /// \warning 不推荐使用该函数，除非你很明白你在做什么
    /// \see sese::StdOutputStreamWrapper::write
    /// \param length 步进大小
    /// \return 步进大小
    int64_t trunc(size_t length) override;

private:
    std::istream &stream;
};

/// @brief 标准库输出包装流
class StdOutputStreamWrapper final
    : public OutputStream {
public:
    explicit StdOutputStreamWrapper(std::ostream &stream);

    /// 向标准库输入流中写入缓存
    /// @param buffer 写入缓存
    /// @param length 写入缓存大小
    /// @warning 除非你很明白你在做什么，否则不要使用这个函数，请确保你传入的 stream.tellp() 不为 -1，否则将无法正确返回实际写入字节数
    /// @return 实际写入大小
    int64_t write(const void *buffer, size_t length) override;

private:
    std::ostream &stream;
    std::streamoff latest;
};

// GCOVR_EXCL_STOP

} // namespace sese::io
