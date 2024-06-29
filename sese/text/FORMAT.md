# 字符串格式化

字符串格式化的核心逻辑位于：

- sese/text/Format.(h|cpp)
- sese/text/FormatOption.(h|cpp)

字符串格式化参考 python 的参数，实际字符串的格式化参数如下:

> {:(wide_char)(align_type)(wide)(.float)(ext_type)}

| 名称 | 内容 | 依赖 | 可选 | 注释 |
|------|-----|-----|----|--------|
| :          | :          |  | 否  | 标准格式化起始符 |
| align_type | <,>,^      |  | 是  | 分别代表左对齐，右对齐和居中 |
| wide_char  | 自定义字符  | align_type | 是  | 用于对齐的填充字符 |
| wide       | 正整数     | align_type | 否  | 对齐的宽度 |
| .float     | .float     | | 是 | 表示浮点数精度 |
| ext_type   | 类型标识符  | | 是 | 一定程度上进行校验并格式化 |

> [!IMPORTANT]
> 与 python 格式化参数最大的不同是对齐参数的顺序是固定的，无论是左对齐、右对齐还是居中。

ext_type 可以取以下值:

- X 大写十六进制
- x 小写十六进制
- o 八进制
- d 十进制
- b 二进制
- % 浮点数百分比
- f 浮点数
- 默认则十进制