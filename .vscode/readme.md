# Usage

To disable the CTest index brought by the CMake extension and automatically enable TestMate, you need to modify the user's configuration file by adding the following settings:

```json
{
    "testMate.cpp.discovery.loadOnStartup": true,
    "cmake.ctest.testExplorerIntegrationEnabled": false
}
```

# Note

This folder is only for default configurations in codespaces and local development with VSCode.

If you use VSCode in any scenario, you should not commit or change this folder.
