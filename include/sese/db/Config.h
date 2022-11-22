#pragma once

#ifdef _WIN32
#define SESE_DB_API __declspec(dllexport)
#else
#define SESE_DB_API
#endif

#include <cstdint>
#include <memory>