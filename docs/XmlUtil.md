# XmlUtil 

## 反序列化

函数签名如下：

```c++
static Element::Ptr deserialize(const Stream::Ptr &inputStream, size_t level) noexcept;
```

其中 level 参数用于指定 xml 嵌套层数允许的最大值，超出此值子对象或当前对象将返回 nullptr。

## 序列化

sese::xml::element 同时拥有 `value` 和 `elements` 两个字段。在使用 XmlUtil::serialize 向文件序列化，且两个字段同时存在时，优先取 `element`—— 即忽略 `value` 字段。