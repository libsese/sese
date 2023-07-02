/**
* @file FileStream.h
* @brief 文件流类（包装）
* @date 2022年4月11日
* @author kaoru
*/
#pragma once

#include "sese/util/Stream.h"

#define BINARY_READ_EXISTED_W "rb,ccs=utf-8"
#define BINARY_WRITE_CREATE_TRUNC_W "wb,ccs=utf-8"
#define BINARY_WRITE_CREATE_APPEND_W "ab,ccs=utf-8"
#define BINARY_BOTH_CREATE_TRUNC_W "wb+,ccs=utf-8"
#define BINARY_BOTH_CREATE_APPEND_W "ab+,ccs=utf-8"
#define TEXT_READ_EXISTED_W "rt,ccs=utf-8"
#define TEXT_WRITE_CREATE_TRUNC_W "wt,ccs=utf-8"
#define TEXT_WRITE_CREATE_APPEND_W "at,ccs=utf-8"
#define TEXT_BOTH_CREATE_TRUNC_W "wt+,ccs=utf-8"
#define TEXT_BOTH_CREATE_APPEND_W "at+,ccs=utf-8"

#define BINARY_READ_EXISTED "rb"
#define BINARY_WRITE_CREATE_TRUNC "wb"
#define BINARY_WRITE_CREATE_APPEND "ab"
#define BINARY_BOTH_CREATE_TRUNC "wb+"
#define BINARY_BOTH_CREATE_APPEND "ab+"
#define TEXT_READ_EXISTED "rt"
#define TEXT_WRITE_CREATE_TRUNC "wt"
#define TEXT_WRITE_CREATE_APPEND "at"
#define TEXT_BOTH_CREATE_TRUNC "wt+"
#define TEXT_BOTH_CREATE_APPEND "at+"

namespace sese {

    /**
     * @brief 文件流类
     */
    class API FileStream : public Stream {
    public:
        using Ptr = std::shared_ptr<FileStream>;

        /// 打开一个文件流
        /// \param fileName 文件名称
        /// \param mode 打开模式
        /// \retval nullptr 打开失败
        /// \retval 文件流对象
        static FileStream::Ptr create(const std::string &fileName, const char *mode) noexcept;
        ~FileStream() = default;

        int64_t read(void *buffer, size_t length) override;
        int64_t write(const void *buffer, size_t length) override;
        void close() override;

        [[nodiscard]] bool good() const { return file != nullptr; }
        int64_t getSeek();
        int32_t setSeek(int64_t offset, int32_t whence);

    private:
        FileStream() noexcept = default;
        FILE *file = nullptr;
    };
}// namespace sese