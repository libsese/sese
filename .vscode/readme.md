# 使用

禁用由 CMake 拓展带来的 CTest 索引并自动启用 TestMate 需要修改用户的配置文件，添加如下配置

```json
{
    "testMate.cpp.discovery.loadOnStartup": true,
    "cmake.ctest.testExplorerIntegrationEnabled": false
}
```

# 注意

此文件夹仅用于 codespaces 和本地开发中的 vscode 默认配置

如果你在任何场景中使用 vscode，都不应该提交或更改此文件夹