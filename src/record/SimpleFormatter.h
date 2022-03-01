#include "Formatter.h"
#include "Config.h"

namespace sese {
    class API SimpleFormatter : public Formatter {
    public:
        /**
         * 默认构造函数
         * @param pattern 格式字符串，默认为 DEFAULT_PATTERN
         * @see DEFAULT_PATTERN
         *
         *     %%   百分号
         *     %li  行号
         *     %lv  日志等级
         *     %f   文件名
         *     %th  线程 ID
         *     %tn  线程名称
         *     %c   完整时间
         *     %m   内容
         */
        explicit SimpleFormatter(const std::string &pattern = DEFAULT_PATTERN) noexcept;

        std::string dump(const Event::Ptr &event) noexcept override;

        [[nodiscard]] const char *getPattern() const noexcept { return this->pattern.c_str(); }

    private:
        std::string pattern;
    };
}// namespace sese