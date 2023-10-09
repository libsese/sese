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
#include "sese/util/TimestampHandler.h"
#include "sese/util/Uuid.h"

namespace sese {
/// Uuid 生成器
class UuidBuilder {
public:
    /// 构造函数
    explicit UuidBuilder() noexcept;

    /// 生成 UUID
    /// \param dest 存放结果的 UUID
    /// \return 是否生成成功
    bool generate(sese::Uuid &dest) noexcept;

protected:
    sese::TimestampHandler timestampHandler;
};
} // namespace sese
