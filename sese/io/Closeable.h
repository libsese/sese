/**
 * @file Closeable.h
 * @date 2023年7月5日
 * @author kaoru
 * @brief 可关闭接口类
 * @version 0.1
 */

#pragma once

namespace sese::io {
/// 可关闭接口
class API Closeable {
public:
    virtual void close() = 0;
};
} // namespace sese::io