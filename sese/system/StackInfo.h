/// \file StackInfo.h
/// \brief 堆栈信息类
/// \author kaoru
/// \date 2023年9月9日

#pragma once

#include <sese/Config.h>

#include <vector>

#if defined(_MSC_VER)
#pragma warning(disable : 4251)
#endif

namespace sese::system {

    /// 子堆栈信息
    struct SubStackInfo {
        uint64_t address{};
        std::string func{};
    };

    /// 堆栈信息
    class API StackInfo {
    public:
        /// 创建一个容器以存储当前的堆栈信息
        /// \param limit 堆栈深度上限
        /// \param skip 跳过深度大小，使用默认值则不包含构造函数过程
        explicit StackInfo(int limit, int skip = WILL_SKIP) noexcept;

        SESE_STD_WRAPPER(begin, stacks)
        SESE_STD_WRAPPER(end, stacks)
        SESE_STD_WRAPPER(rbegin, stacks)
        SESE_STD_WRAPPER(rend, stacks)
        SESE_STD_WRAPPER(empty, stacks)
        SESE_STD_WRAPPER(size, stacks)

        static int WILL_SKIP;

    protected:
        /// 此函数在 Windows 上用于去除模板实参信息，其他系统则用于解析符号名称
        static std::string decodeSymbolName(const std::string &str) noexcept;

        std::vector<SubStackInfo> stacks;
    };

}// namespace sese::system
