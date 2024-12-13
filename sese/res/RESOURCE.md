# Embedded Resource

Managing and embedding resource files is a common requirement, especially in embedded systems or applications that need to package static resources. To simplify this process, tools are often used to convert file contents into a format that C++ can directly use. The two methods provided by the framework are Bundler and Classic Mode.

Bundler is a flexible resource packaging tool that converts file contents into C++ code, allowing projects to directly reference resources, avoiding the traditional file-reading process. Similarly, Classic Mode uses an approach similar to `xxd`, converting file contents into byte streams or strings and embedding them into C++ code, providing a simple way to manage and access embedded resources. While both methods serve similar goals, they differ in their implementation and applicable scenarios.

## Bundler (Recommended)

Bundler is a resource file code generation tool used to embed specified binary files into a C++ project. It supports Windows, Linux (ELF format), and macOS (Mach-O format). Users can quickly generate platform-adapted code and intermediate files by specifying the resource file paths and generation options, without handling low-level details.

---

### Feature Overview

1. **Windows (RC)**
   Generates `.rc` and `.h` files for embedding binary resources into Windows applications.

2. **Linux (ELF)**
   Generates `.h` and `.cpp` files, and links binary files using `ld` to generate `.o` object files.

3. **macOS (Mach-O)**
   Generates `.h` files and `.link_options` files, supporting Mach-O format linking.

---

### Usage

- **Resource File**
  JSON format, defining the paths of the binary files to be embedded. For example:

  ```json
  {
      "binaries": {
          "File1": "path/to/file1.bin",
          "File2": "path/to/file2.bin"
      }
  }
  ```

- **CMake File**
  Specifies the target that includes the resource, and the target must be an executable file. Note that a target can only have one resource description file to avoid resource ID conflicts.

  ```cmake
  target_include_resource(main R.json)
  ```

- **C++ Source File**

  ```cpp
  #include <sese/res/BundlerResource.h>
  #include "R.h"

  auto res = sese::res::BundlerResource<R>();
  auto txt = res.getBinary(R::Binaries::File1);
  char buffer[1024]{};
  auto l = txt->read(buffer, 1024);
  ```

---

### Feature Details

1. **Filename Normalization**
   Automatically removes illegal characters from filenames, replacing unprintable characters with underscores. All resource identifiers are in the form of enumerations, providing high robustness.

2. **Windows Numbering Definition**
   Uses constant enumeration numbering starting from 1000, with each binary resource having a unique identifier. Other platforms start from 0.

3. **ELF Binary Mapping**
   Uses the `ld` command to generate a binary file link map, supporting the generation of intermediate packaging files.

4. **Mach-O Support**
   Implements embedded resources based on the `section` mechanism.

5. **Binary File Information Support**
   Similarly, we also support adding product information such as author, product name, version, etc., to Windows binary files using a JSON file.

   ```json
   {
       "author": "your name",
       "productName": "xxx",
       "internalName": "xxx",
       "version": "2.3.0",
       "description": "hello",
       "copyright": "(c) 2024-20xx xxx"
   }
   ```

   ```cmake
   target_manifest(main manifest.json)
   ```

---

### Notes

1. **Resource File Paths**
   All resource file paths must be relative to `--base_path`.

2. **Tool Dependencies**
    - Windows environment requires a resource file compiler (e.g., `rc.exe`).
    - Linux environment requires the installation of the `ld` tool.
    - macOS environment requires a compiler toolchain supporting Mach-O format.

---

## Classic Mode

The `res_pack.py` script is used to package file contents into a C++ resource header file, converting files into embedded resource strings in a manner similar to `xxd`. Below is a detailed summary of this tool, highlighting its similarities to and limitations compared to `xxd`.

---

#### 1. **Function Description**
- **Objective:** Convert file contents into C++ resource files for embedding into the program.
- **Method:** Similar to `xxd`, the tool converts file contents into C++ string literals or byte streams, suitable for embedded resource processing.
- **Mode Support:**
    - **Resource Package Mode (-r):** Recursively packages multiple files into a resource file.
    - **Resource Stream Mode (-s):** Single-file resource stream for embedding one file into C++ code.
- **Summary:** We do not recommend using this mode. Bundler is a better choice in every aspect. Further demonstrations will not be provided; you may refer to the `TestResource` test case for usage.

---

#### 2. **Similarity to `xxd`**
- **File Conversion:** Similar to how `xxd` converts files into C arrays, this tool converts file contents into C++ string literals or binary streams.
- **Embedability:** Just like the C arrays generated by `xxd`, the generated resources can be directly embedded into the program for easy use.
- **Code Generation:** Automatically generates a C++ header file with resource declarations and macros, making resource management and access more efficient.

---

#### 3. **Key Points**
- **Performance:** As it is implemented in Python, the tool may be slower when processing large files or numerous files, especially in resource package mode.
- **Poor Support for Large Files:** Converting files into string literals may cause issues with very large files, especially binary files with special characters. `xxd` also faces similar challenges.
- **Code Generation Size Limitation:** The generated code may encounter compilation issues due to large strings or binary data, particularly in projects with a large number of resources.
- **Binary File Handling:** Binary data processing is relatively simplified, and some control characters may be escaped as strings, potentially affecting the integrity of the original data.

---

#### 4. **Scenarios**
- **Suitable Scenarios:**
    - Embedding small resource files such as icons, configuration files, scripts, etc., particularly in projects with limited resources and few files.
    - C++ projects needing to integrate precompiled static resources, ideal for generating unified resource header files.
- **Unsuitable Scenarios:**
    - Projects that require handling a large number of large files or efficiently embedding large-scale binary data.
    - Scenarios with strict performance requirements, especially when dealing with large amounts of file resources.
