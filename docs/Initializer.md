# Initializer

程序生命周期内全局对象的保序初始化与销毁。

## 顺序

当前初始化顺序

- Encoding Converter

- Logger
- Test
- CPU Info
- Socket (Windows Only)

销毁顺序与初始化顺序相反

## 自定义的初始化

如果程序对上述模块和功能有一定的依赖（尤其是顺序），建议手动添加自定义的初始化任务。

详情参考单元测试 [Test Initializer](../test/TestInitializer/main.cpp)。