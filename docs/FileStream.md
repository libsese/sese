# File Stream

## 打开模式

- 打开模式对照表

  | DEFINE                     | ::fopen |
  | -------------------------- | ------- |
  | BINARY_READ_EXISTED        | rb      |
  | BINARY_WRITE_CREATE_TRUNC  | wb      |
  | BINARY_WRITE_CREATE_APPEND | ab      |
  | BINARY_BOTH_CREATE_TRUNC   | wb+     |
  | BINARY_BOTH_CREATE_APPEND  | ab+     |
  | TEXT_READ_EXISTED          | rt      |
  | TEXT_WRITE_CREATE_TRUNC    | wt      |
  | TEXT_WRITE_CREATE_APPEND   | at      |
  | TEXT_BOTH_CREATE_TRUNC     | wt+     |
  | TEXT_BOTH_CREATE_APPEND    | at+     |

  \*\* 注意 \*\*："_W" 后缀表示添加 "ccs=utf-8"。默认 linux 下不需要使用，为 Windows 读取 UTF-8 准备

- 关键字释义
  
  - READ 只读
  - WRITE 只写
  - BINARY 二进制模式
  - TEXT 文本模式
  - EXISTED 文件必须存在
  - CREATE 不存在则创建
    - TRUNC 截断
    - APPEND 追加

## 文本模式

已知文本模式做了以下处理

- 处理特定平台下文件的特殊的格式，例如 EOF 标识符的判断。
- 处理不同平台下的换行符，"\n"、"\r\n"、"\r"等。

如果需要跨平台读取 UNICODE 字符，请参考 [UniReader](UniReader.md)。
