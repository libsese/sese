# Contribution Guide

## Project Configuration

Project configuration uses CMake + vcpkg. CMake is responsible for building, and vcpkg is responsible for managing dependencies.

### Typical Configuration Steps:

- **Download the project**

  You should choose to use `git clone` to download the project rather than directly downloading the source code archive because we need version management.
  ```bash
  git clone https://github.com/libsese/sese.git
  ```

- **Configure CMake**

  Note: If you use a native package manager, you do not need to configure vcpkg or specify CMAKE_TOOLCHAIN_FILE. Just set `SESE_USE_NATIVE_MANAGER=ON`.

  First, make sure that you have the vcpkg tool configured on your machine.
  There are several ways to configure CMake. If you are using VSCode, you might need to install the CMake plugin and add the following option to your project configuration:

  `-DCMAKE_TOOLCHAIN_FILE=$VCPKG_ROOT/scripts/buildsystems/vcpkg.cmake`

  Using Visual Studio or CLion is similar to configuring VSCode. Besides the above option, you also need to add:

  `-DSESE_BUILD_TEST=ON`

  This will enable the build unit tests, which will automatically add the Google Test dependency and include the project's unit test targets.

  The complete CMake build command should look like this:
  ```bash
  cmake -DCMAKE_TOOLCHAIN_FILE=$VCPKG_ROOT/vcpkg/scripts/buildsystems/vcpkg.cmake -DSESE_BUILD_TEST=ON
  ```

- **Code Standards**

  Style, format, and checks should be enabled. The project includes **.clang-format**, **.clang-tidy**, and **.editconfig** files.
  These are necessary for the maintenance and development of new features in the project.

## Preset Development Environment

Preset configurations should now refer to **CMakePresets.json**. If you are using an outdated version of CMake or an IDE, you might need to manually configure these development options.

For those who prefer using development containers, the project provides development containers. Just enable the Unix workspace within the container.

Please note that on macOS, you might need to re-specify the location of the CMake generator in the workspace because Ninja installed via Homebrew is usually not located at /usr/bin/ninja. Use

```bash
where ninja
```
to find the full path to Ninja.

## About ARM Architecture

> [!WARNING]
> ARM is not the primary supported architecture at the moment. The project can run on ARM to a certain extent.
> If you insist on using related devices for development and operation, we are not responsible for the results. However, we welcome developers with relevant experience to contribute compatibility code.
> The following content provides a **simple** and **non-rigorous** feasible ARM64 emulation scheme for x86_64/amd64 developers.

Add cross-architecture support for Docker, which **may** require you to manually install `qemu` components. This is not demonstrated here. For more details, please refer to [tonistiigi/binfmt](https://github.com/tonistiigi/binfmt).

```bash
docker run --privileged --rm tonistiigi/binfmt --install arm64
```

[ubuntu-arm64.dockerfile](./docker/ubuntu-arm64.dockerfile) provides a basic runtime environment based on ARM64 Ubuntu (excluding database software). You can build the image directly using Docker.

```bash
docker build -f docker/ubuntu-arm64.dockerfile .
```

Using this image, you can fully build and run related test cases.

## Branch Management

`main` is the main branch of this project. Most of the time, contributions are not allowed to be uploaded here without permission.

To modify the code, you can choose to fork the project to your personal account and initiate a pull request to the upstream after completing development.
If you are an organization member, you have permission to create a new development branch in this repository. The branch name should typically follow these formats:

`dev-[username]`,
`dev-feature`,
`dev-new-feature`,
`fix-bug`, etc.

## Submission Process

After completing development, if the commits in your local or personal repository are too complex or cumbersome, you should compress the commits appropriately before initiating a pull request to the upstream.

When you complete new features or fixes, you should ensure that all unit tests in the project pass successfully. Especially in GitHub workflows, network, system, and other environments may not be consistent with your local machine, which may cause unit tests to pass locally but fail in the workflow.

Once confirmed, you can initiate a pull request to the upstream, describing clearly the changes you made to the existing code and the new code's functionality. You can also choose to send an email to existing contributors or staff of the project, detailing the work you have completed.

The format of the commit message can be summarized as follows:

> { commit type }: { title }
>
> { detail (optional) }

Common commit types include:

- feat - New feature
- fix - Bug fix
- perf - Performance improvement
- refactor - Refactoring
- build - Build changes
- chore - Chores
- docs - Documentation
- style - Code style changes
- test - Test case changes

## Issues and Requirements

Before writing specific code, you should analyze the problem you want to solve and clarify your requirements. Then analyze whether the existing code already contains similar functionality.

For new features, you need to ensure they fall within the scope of the project's functionality. For changes to existing implementations, you need to ensure that these changes do not cause significant incompatibility with existing implementations.
If you have to make some destructive changes to some underlying code, you should reimplement a version, retaining the old and adding new implementations through **namespace** + **using/typedef**, etc.

Typically, a change should be atomic and local, meaning you should only modify one function at a time. If you have to modify B when implementing A, you should first contact the relevant personnel for their opinion and test that the changes to B will not affect the original use of B.

The project typically does not allow new dependencies to be directly added. Introducing new dependencies requires detailed research and thorough discussion.

## Contribution Types

The types of contributions can be broadly referred to in the [Submission Process](#submission-process).

## Testing Requirements

The project uses GoogleTest for testing. Tests do not require 100% coverage but should provide the most comprehensive coverage for the most commonly used features. Relatively insignificant features can selectively not be tested for some non-extreme scenarios.

For some functions or branches that may be difficult to test, it is best to add detailed comments to indicate why coverage was not achieved, potential risks, and the response plan if the situation occurs.

## Release Process

This part of the specification applies to core project administrators and needs to be strictly followed:

1. Change the project version number, modify `CMakeLists.txt` and `vcpkg.json`, tag the commit, and it will automatically trigger `update_portfile.py` and perform pre-release testing.
2. Failure of the test will terminate the release. At this time, name the new tag following the `alpha.$(n + 1)` format. For official releases, increase the patch number or withdraw the current version.
3. Tags containing `beta` or `alpha` will not be set as the final version.
4. Maintaining a new version (if necessary, mostly rolling release) requires creating a new branch. The branch name format is as follows `release-$(version)`, and administrators need to manually release it.

## Contact Information

If you have questions about some parts of the code or other issues, you can first try to contact the contributor of the code snippet for help.

Or contact the project's [original author](https://github.com/shiinasama/) for inquiries and available assistance.
