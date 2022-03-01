#pragma once

#ifdef _WIN32
#define pid_t int

#ifdef WINDOWS_DLL
#define API __declspec(dllexport)
#endif
#endif


#ifdef __linux__
#define API
#endif

// String Buffer 单元因子
#define STRING_BUFFER_SIZE_FACTOR 1024
// 日志格式化器匹配格式
#define DEFAULT_PATTERN "%tm %lv %tn:%th %f:%li>%m"
// 空格字符
#define SPACE_CHARS " \n\r\t\v\b"