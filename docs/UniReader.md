# UniReader

跨平台的 Unicode 字符读取，主要处理 Windows 下读取功能的差异。

原生读取数据为 wchar[]，后转为 char[] 统一处理。

参考实现 src/native/Windows/UniReader.cpp

<iframe width=90% src="../src/native/Windows/config/UniReader.cpp"/>