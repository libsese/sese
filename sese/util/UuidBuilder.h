/**
 * \file UuidBuilder.h
 * \author kaoru
 * \date 2022.12.24
 * \version 0.1
 * \brief UUID 生成相关
 */

#pragma once

#ifdef WIN32
#pragma warning(disable : 4251)
#endif

#include "sese/Config.h"
#include "SimpleUuid/TimestampHandler.h"
#include "SimpleUuid/Uuid.h"

namespace sese {
    class API UuidBuilder;
}// namespace sese

/// Uuid 生成器
class sese::UuidBuilder {
public:
    /// 构造函数
    explicit UuidBuilder() noexcept;

    /// 生成 UUID
    /// \param dest 存放结果的 UUID
    /// \return 是否生成成功
    bool generate(uuid::Uuid &dest) noexcept;

protected:
    uuid::TimestampHandler timestampHandler;
};