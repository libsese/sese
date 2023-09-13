/**
* @file MD5Util.h
* @brief MD5 算法类
* @author kaoru
* @date 2022年4月22日
*/
#pragma once

#include "sese/io/Stream.h"

namespace sese {

    /**
    * @brief MD5 算法类
    */
    class API MD5Util {
    public:
        /// 工具所用输出流
        using OutputStream = sese::io::OutputStream;
        /// 工具所用输入流
        using InputStream = sese::io::InputStream;

        /**
        * @brief 生成 MD5 信息
        * @param input 摘要来源
        * @param output MD5 数据输出
        */
        static void encode(const InputStream::Ptr &input, const OutputStream::Ptr &output) noexcept;
        /**
        * @brief 生成 MD5 信息
        * @param input 摘要来源
        * @param output MD5 数据输出
        */
        static void encode(InputStream *input, OutputStream *output) noexcept;

        /**
        * @brief 生成 32 位大写 MD5 字符串
        * @param input 摘要来源
        * @param isCap 指示字符串字母是否大写
        * @return 返回生成的字符串，生成失败则为 nullptr
        */
        static std::unique_ptr<char[]> encode(const InputStream::Ptr &input, bool isCap = true) noexcept;
        /**
        * @brief 生成 32 位大写 MD5 字符串
        * @param input 摘要来源
        * @param isCap 指示字符串字母是否大写
        * @return 返回生成的字符串，生成失败则为 nullptr
        */
        static std::unique_ptr<char[]> encode(InputStream *input, bool isCap = true) noexcept;

    private:
        /// 用于进行变换操作
        static void transform(uint32_t *res, uint8_t *buffer) noexcept;

        static const uint32_t A = 0x67452301;
        static const uint32_t B = 0xefcdab89;
        static const uint32_t C = 0x98badcfe;
        static const uint32_t D = 0x10325476;

        static const uint32_t S11 = 7;
        static const uint32_t S12 = 12;
        static const uint32_t S13 = 17;
        static const uint32_t S14 = 22;

        static const uint32_t S21 = 5;
        static const uint32_t S22 = 9;
        static const uint32_t S23 = 14;
        static const uint32_t S24 = 20;

        static const uint32_t S31 = 4;
        static const uint32_t S32 = 11;
        static const uint32_t S33 = 16;
        static const uint32_t S34 = 23;

        static const uint32_t S41 = 6;
        static const uint32_t S42 = 10;
        static const uint32_t S43 = 15;
        static const uint32_t S44 = 21;

        constexpr static const unsigned char PADDING[64] = {0x80};

        static inline uint32_t F(uint32_t x, uint32_t y, uint32_t z) noexcept;
        static inline uint32_t G(uint32_t x, uint32_t y, uint32_t z) noexcept;
        static inline uint32_t H(uint32_t x, uint32_t y, uint32_t z) noexcept;
        static inline uint32_t I(uint32_t x, uint32_t y, uint32_t z) noexcept;

        static inline uint32_t FF(uint32_t a, uint32_t b, uint32_t c, uint32_t d, uint32_t x, uint32_t s, uint32_t t) noexcept;
        static inline uint32_t GG(uint32_t a, uint32_t b, uint32_t c, uint32_t d, uint32_t x, uint32_t s, uint32_t t) noexcept;
        static inline uint32_t HH(uint32_t a, uint32_t b, uint32_t c, uint32_t d, uint32_t x, uint32_t s, uint32_t t) noexcept;
        static inline uint32_t II(uint32_t a, uint32_t b, uint32_t c, uint32_t d, uint32_t x, uint32_t s, uint32_t t) noexcept;
    };
}// namespace sese