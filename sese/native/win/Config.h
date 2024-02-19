/**
 * @file Config.h
 * @brief Windows 平台配置文件
 * @author kaoru
 * @date 2022年3月28日
 */
#pragma once

#if !defined(_WIN32_WINNET) || _WIN32_WINNI < 0x0602
#define _WIN32_WINNNT 0x0602
#endif

#ifndef WINVER
#define WINVER 0x0602
#endif

#ifndef NOMINMAX
#define NOMINMAX
#endif

#include <winsock2.h>
#include <windows.h>

#undef min
#undef max

#pragma warning(disable : 4819)

#if defined(_M_X64)
#define SESE_ARCH_X64
#elif defined(_M_ARM64)
#define SESE_ARCH_ARM64
#else
#error only support x86_64 & arm64
#endif

/// C++ 版本标识符
#define SESE_CXX_STANDARD _MSVC_LANG

/// 可导出类型标识符 - 现在已弃用
#define API

/// pid_t 格式化占位符
#define PRIdTid "u"

#ifndef __MINGW32__
/// 忽略大小写比较
#define strcasecmp strcmpi
#endif

#ifndef timegm
#define timegm _mkgmtime
#endif

#if defined(__MINGW32__)
#define SESE_FILENAME (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)
#else
#define SESE_FILENAME (strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__)
#endif

namespace sese {
/// 进程 ID 标识符
using pid_t = DWORD;
/// 线程 ID 标识符
using tid_t = uint32_t;
/// Native Socket 类型
using socket_t = SOCKET;
} // namespace sese

/// WSA 错误映射

#undef EINTR
#undef EBADF
#undef EACCES
#undef EFAULT
#undef EINVAL
#undef EMFILE
#undef EWOULDBLOCK
#undef EINPROGRESS
#undef EALREADY
#undef ENOTSOCK
#undef EDESTADDRREQ
#undef EMSGSIZE
#undef EPROTOTYPE
#undef ENOPROTOOPT
#undef EPROTONOSUPPORT
#undef ESOCKTNOSUPPORT
#undef EOPNOTSUPP
#undef EPFNOSUPPORT
#undef EAFNOSUPPORT
#undef EADDRINUSE
#undef EADDRNOTAVAIL
#undef ENETDOWN
#undef ENETUNREACH
#undef ENETRESET
#undef ECONNABORTED
#undef ECONNRESET
#undef ENOBUFS
#undef EISCONN
#undef ENOTCONN
#undef ESHUTDOWN
#undef ETOOMANYREFS
#undef ETIMEDOUT
#undef ECONNREFUSED
#undef ELOOP
#undef ENAMETOOLONG
#undef EHOSTDOWN
#undef EHOSTUNREACH
#undef ENOTEMPTY
#undef EPROCLIM
#undef EUSERS
#undef EDQUOT
#undef ESTALE
#undef EREMOTE

#define EINTR WSAEINTR
#define EBADF WSAEBADF
#define EACCES WSAEACCES
#define EFAULT WSAEFAULT
#define EINVAL WSAEINVAL
#define EMFILE WSAEMFILE
#define EWOULDBLOCK WSAEWOULDBLOCK
#define EINPROGRESS WSAEINPROGRESS
#define EALREADY WSAEALREADY
#define ENOTSOCK WSAENOTSOCK
#define EDESTADDRREQ WSAEDESTADDRREQ
#define EMSGSIZE WSAEMSGSIZE
#define EPROTOTYPE WSAEPROTOTYPE
#define ENOPROTOOPT WSAENOPROTOOPT
#define EPROTONOSUPPORT WSAEPROTONOSUPPORT
#define ESOCKTNOSUPPORT WSAESOCKTNOSUPPORT
#define EOPNOTSUPP WSAEOPNOTSUPP
#define EPFNOSUPPORT WSAEPFNOSUPPORT
#define EAFNOSUPPORT WSAEAFNOSUPPORT
#define EADDRINUSE WSAEADDRINUSE
#define EADDRNOTAVAIL WSAEADDRNOTAVAIL
#define ENETDOWN WSAENETDOWN
#define ENETUNREACH WSAENETUNREACH
#define ENETRESET WSAENETRESET
#define ECONNABORTED WSAECONNABORTED
#define ECONNRESET WSAECONNRESET
#define ENOBUFS WSAENOBUFS
#define EISCONN WSAEISCONN
#define ENOTCONN WSAENOTCONN
#define ESHUTDOWN WSAESHUTDOWN
#define ETOOMANYREFS WSAETOOMANYREFS
#define ETIMEDOUT WSAETIMEDOUT
#define ECONNREFUSED WSAECONNREFUSED
#define ELOOP WSAELOOP
#define ENAMETOOLONG WSAENAMETOOLONG
#define EHOSTDOWN WSAEHOSTDOWN
#define EHOSTUNREACH WSAEHOSTUNREACH
#define ENOTEMPTY WSAENOTEMPTY
#define EPROCLIM WSAEPROCLIM
#define EUSERS WSAEUSERS
#define EDQUOT WSAEDQUOT
#define ESTALE WSAESTALE
#define EREMOTE WSAEREMOTE