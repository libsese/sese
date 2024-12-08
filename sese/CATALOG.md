# Directory Explanation

**internal** is the **Internal Implementation Directory**.
Its contents are not exposed to any project outside of Core and can be specifically used for implementing interfaces.

**native** is the **System Platform Implementation Directory**,
which contains implementations specific to certain platforms and is typically only exposed to those platforms.

- darwin usually refers to Apple's macOS platform.
- win generally refers to Microsoft's Windows platform.
- linux typically refers to various Linux distributions.
- unix represents the intersection of darwin and linux.

**test** is the **Unit Test Directory**.
The root directory contains the main entry point for tests and ordinary test files.
The TestXXX directory contains test cases composed of multiple source files or targets.
The Data directory stores non-source code data needed for testing.

**benchmark** is the **Performance Test Directory**.
All tests are named in the format BM_Xxx and share the enablement conditions with unit tests.

**types** is the **Template Type Extraction Tools Directory**, with a different code style from the main project.
