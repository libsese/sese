/// \file Config.h
/// \author kaoru
/// \brief 配置文件
/// \version 0.1
#pragma once

#ifdef _WIN32
#define SESE_DB_API __declspec(dllexport)
#else
#define SESE_DB_API
#endif

#ifdef _MSC_VER
#pragma warning(disable : 4251)
#endif

#include <cstdint>
#include <memory>