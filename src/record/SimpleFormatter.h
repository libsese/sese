#include "Config.h"
#include "Formatter.h"

namespace sese {
    class API SimpleFormatter : public Formatter {
    public:
        /**
         * 默认构造函数
         * @param pattern 格式字符串，默认为 DEFAULT_RECORD_TEXT_PATTERN
         * @see DEFAULT_RECORD_TEXT_PATTERN
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
        explicit SimpleFormatter(const std::string &textPattern = DEFAULT_RECORD_TEXT_PATTERN,
                                 const std::string &timePattern = DEFAULT_RECORD_TIME_PATTERN) noexcept;

        std::string dump(const Event::Ptr &event) noexcept override;

        [[nodiscard]] const char *getTextPattern() const noexcept { return this->textPattern.c_str(); }

        [[nodiscard]] const char *getTimePattern() const noexcept { return this->timePattern.c_str(); }

    private:
        std::string textPattern;
        std::string timePattern;
    };
}// namespace sese