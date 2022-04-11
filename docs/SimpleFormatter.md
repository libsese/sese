# Simple Formatter

## 格式匹配字符串

- 匹配格式对照表
  | 匹配格式 | 释义              |
  | -------- | ----------------- |
  | %%       | 转义符号本身，'%' |
  | %li      | 行号              |
  | %lv      | 日志等级          |
  | %ln      | 日志器名称        |
  | %fn      | 文件名称          |
  | %fi      | 过滤器名称        |
  | %th      | 线程 ID           |
  | %tn      | 线程名称          |
  | %c       | 时间[[0]](#jmp0)  |
  | %m       | 日志内容          |
  
- 默认格式

  ```log
  [%ln] %c %lv %fi %tn:%th> %m
  ```

  输出示例

  ```log
  [ROOT] 2022-04-11T20:27:21.785Z DEBUG fLOGGER Main:5477> Hello
  ```



<span id="jmp0"/>

时间格式化相关请参看 [DateTimeFormatter 日期时间格式化器](./DateTimeFoematter.md)

