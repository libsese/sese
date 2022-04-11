/**
* @file FileStream.h
* @brief 文件流类（包装）
* @date 2022年4月11日
* @author kaoru
*/
#pragma once
#include "Stream.h"
#include <string>

namespace sese {

    /**
     * @brief 文件流类
     */
    class API FileStream : public Stream {
    public:
        struct Mode {
        public:
            constexpr static const char *BINARY_READ_EXISTED = "rb";
            constexpr static const char *BINARY_WRITE_CREATE_TRUNC = "wb";
            constexpr static const char *BINARY_WRITE_CREATE_APPEND = "ab";
            constexpr static const char *BINARY_BOTH_CREATE_TRUNC = "wb+";
            constexpr static const char *BINARY_BOTH_CREATE_APPEND = "ab+";
            constexpr static const char *TEXT_READ_EXISTED = "rt";
            constexpr static const char *TEXT_WRITE_CREATE_TRUNC = "wt";
            constexpr static const char *TEXT_WRITE_CREATE_APPEND = "at";
            constexpr static const char *TEXT_BOTH_CREATE_TRUNC = "wt+";
            constexpr static const char *TEXT_BOTH_CREATE_APPEND = "at+";
        };

    public:
        FileStream(const std::string &fileName, const char *mode);
        ~FileStream() = default;

        int64_t read(void *buffer, size_t length) override;
        int64_t write(void *buffer, size_t length) override;
        void close() override;

        [[nodiscard]] bool good() const { return file != nullptr; }
        int64_t getSeek();
        int32_t setSeek(int64_t offset, int32_t whence);

    private:
        FILE *file = nullptr;
    };
}// namespace sese