# String Formatting

The core logic for string formatting is located in:

- sese/text/Format.(h|cpp)
- sese/text/FormatOption.(h|cpp)

String formatting is based on Python's parameters. The actual string formatting parameters are as follows:

> {:(wide_char)(align_type)(wide)(.float)(ext_type)}

| Name       | Content       | Dependency   | Optional | Comment                                       |
|------------|---------------|--------------|----------|-----------------------------------------------|
| :          | :             |              | No       | Standard format starting character            |
| align_type | <,>,^         |              | Yes      | Represent left align, right align, and center |
| wide_char  | Custom char   | align_type   | Yes      | Character used for alignment padding          |
| wide       | Positive int  | align_type   | No       | Alignment width                               |
| .float     | .float        |              | Yes      | Floating point precision                      |
| ext_type   | Type specifier|              | Yes      | For validation and formatting to some extent  |

> [!IMPORTANT]
> The biggest difference from Python's formatting parameters is that the order of alignment parameters is fixed, regardless of left, right, or center alignment.

ext_type can take the following values:

- X for uppercase hexadecimal
- x for lowercase hexadecimal
- o for octal
- d for decimal
- b for binary
- % for floating point percentage
- f for floating point
- Default is decimal
