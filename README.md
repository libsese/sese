<h1 align="center">Sese Library</h1>
<div align="center">
<img src="logo.svg" width="128" height="128" alt="logo"/>
</div>
<div align="center">
<img src="https://img.shields.io/static/v1?label=license&message=Apache-2.0&color=blue&logo=Apache" alt="license"/>
<img src="https://img.shields.io/static/v1?label=language&message=C%2B%2B%2017&color=blue&logo=cplusplus" alt="lang"/>
<img src="https://img.shields.io/static/v1?label=build%20system&message=CMake&color=blue&logo=cmake" alt="buildsystem"/>
<br>
<img alt="CodeFactor Status" src="https://www.codefactor.io/repository/github/libsese/sese-core/badge"/>
<img alt="GitHub Workflow Status (with event)" src="https://img.shields.io/github/actions/workflow/status/libsese/sese-core/windows-latest.yml?label=Windows&logo=windows">
<img alt="GitHub Workflow Status (with event)" src="https://img.shields.io/github/actions/workflow/status/libsese/sese-core/ubuntu-latest.yml?label=Ubuntu&logo=ubuntu">
<img alt="GitHub Workflow Status (with event)" src="https://img.shields.io/github/actions/workflow/status/libsese/sese-core/macos-latest.yml?label=macOS&logo=apple">
<br>
</div>

## SESE-CORE

[中文总览](README.zh_CN.md)

SESE-CODE is a cross-platform foundational project that utilizes the C++17 language standard, employs CMake as its build
system, and integrates vcpkg as the package manager. It has versatile tools, spanning from system-level development to
web development and beyond.

Currently, the project is compatible with Windows, macOS, and various Linux distributions running on the x86_64
architecture. However, compatibility with the ARM architecture has not been verified.

The project's primary objective is to serve as a supplementary resource to the standard library, akin to libraries like
folly and boost. It provides commonly used features such as asynchronous programming, logging, multithreading, binary
interface management, and more.

## BUILD

If you're using vcpkg to manage dependencies, you don't need to worry about the specific build process of this project.
You just need to import the private vcpkg-registry, complete the vcpkg.json file, and add the vcpkg toolchain.

In addition, if you're compiling and installing it or if you're a developer working on this project locally for
debugging purposes, you'll also need to use vcpkg. Clone the project, configure it, add the vcpkg toolchain, and set
SESE_BUILD_TEST=ON to build unit tests.

You can automatically configure the current project into the 'build' directory using the following script:

```bash
python3 scripts/config.py
```

## CODESPACES

Using CODESPACES, you only need to create a container with the project's default settings. The .vscode/settings.json
file provides the default configuration that the container can use directly.

## DOCKER CONTAINER

The project also provides a Dockerfile to build a containerized development environment that can be used directly. For
specific details, please refer to the docker/readme.md file; we won't go into further details here.

## UNIT TEST

The project has chosen googletest as its unit testing framework, and some tests have disabled the SIGPIPE signal to
ensure smooth execution. When using gdb or lldb for debugging, it may automatically load the **.gdbinit**/**.lldbinit**
files in the project, which suppress all SIGPIPE signals. To run all tests, simply navigate to the build directory and
execute ctest.

## TEST REPORT

Generating test reports requires compiler support and setting flags to generate report data. To generate the report,
you'll need gcovr, which can be installed via pip or your system's package management system. For filtering out
deprecated or obsolete implementations, a **gcovr.cfg** file has been provided. You can use the following command to
generate test reports directly in the 'coverage' directory.

```bash
python3 scripts/make_coverage_report.py
```

Sometimes, you may need to use specific options of gcovr to select a particular version of the gcov executable.

## WORKFLOWS

For all three supported platforms, there are automated test workflows in place. In the test_coverage_job on Ubuntu, a
test report file named CoverageReport.zip will be generated.

## MORE

If you'd like to learn more, please click [here(Chinese Proficiency Required)](https://libsese.github.io/sese-docs/#/).