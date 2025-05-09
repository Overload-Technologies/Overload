# Contributing to Overload
First of all, thank you for your interest in Overload! Any contribution is welcome, including:
- Reporting a bug
- Submitting a fix
- Proposing new features
- Improving code quality

## We Develop with Github
We use GitHub to host code, track issues and feature requests, and accept pull requests.

## All Code Changes Happen Through Pull Requests
Pull requests are the best (and only) way to propose changes to the codebase. We actively welcome your pull requests.

To create a pull request:

1. Fork the repository.
2. Create your branch from the `main` branch.
3. Review your code before submitting (conduct build and quality checks).
4. Create a pull request targeting the `main` branch.

## Any contributions you make will be under the MIT Software License
In short, when you submit code changes, your contributions are understood to be under the same [MIT License](http://choosealicense.com/licenses/mit/) that covers the project. Feel free to contact the maintainers if that's a concern.

## Report bugs using Github's issues
We use GitHub issues to track bugs. Report a bug by opening a new issue, it's that easy!

## Coding Conventions
### File Header
Every file must include the following header:
```cpp
/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/
```

### Naming
| | |
|-|-|
| N.1 | Custom Types (*structs*, *classes*, *enums*, *typedefs*) use the `UpperCamelCase` notation. |
| N.2 | Functions & methods use the `UpperCamelCase` notation. |
| N.3 | Variables use the `lowerCamelCase` notation (unless specified otherwise, see below). |
| N.4 | Private member variables are prefixed with `m_` (e.g. `m_elapsedTime`). |
| N.5 | Function parameters are prefixed with `p_` (e.g. `p_enabled` ). |
| N.6 | Compile-time constants are prefixed with `k` followed by `UpperCamelCase` (e.g. `kPi`). |
| N.7 | Do not use abbreviations. |
| N.8 | Interfaces are prefixed with `I`, abstract classes with `A`, and enum types with `E`. |
| N.9 | Enum values are in `SHOUT_CASE`. |

### Formatting
| | |
|-|-|
| F.1 | Curly braces must be placed on their own lines, except for the opening brace of lambdas. |
| F.2 | Code must be indented using tabs. |
| F.3 | Keep a low indentation level (use early returns, split functionalities, reduce complexity). |
| F.4 | Always use braces for scopes (*if*, *for*, *while*, etc.). |
| F.5 | Avoid single line `if` statements (e.g. `if (condition) return 0;`). |
| F.6 | Always end your files with an empty line. |
| F.7 | Avoid aligning variable names and values using tabulations. |

### Classes
| | |
|-|-|
| C.1 | Members are ordered by access level (`public`, `protected`, `private`). |
| C.2 | Methods always come first, followed by member variables. |

### Includes
| | |
|-|-|
| I.1 | Includes are in lexicographic order with lowercase precedence (e.g. `aa`, `aA`, `Aa`, `AA`). |
| I.2 | Included file paths are fully specialized (`Project/Namespace/File.h`). |
| I.3 | Include statements use brackets (`<>`) over quotation marks (`""`). |

### Documentation
| | |
|-|-|
| D.1 | Document public types, methods, and functions using the [Javadoc style](https://en.wikipedia.org/wiki/Javadoc). |

### General
| | |
|-|-|
| G.1 | Almost always avoid *macros*. Use `constexpr` whenever possible. |
| G.2 | Always use forward slashes when it comes to paths. |
| G.3 | Prefer anonymous functions over private methods when possible. |
| G.4 | Avoid declaring multiple classes in a single header file. |
| G.5 | Prefer stack allocations over heap allocations. |
| G.6 | Almost always avoid manual heap allocations (`malloc`, `new`, `delete`). |
| G.7 | Avoid raw pointers (use smart pointers when required). |
| G.8 | Use `std::array` over dynamic collections (e.g. `std::vector`) when the size is fixed. |
| G.9 | Use `std::string_view`, `std::span`, and *const references* whenever possible. |
| G.10| Prefer composition over inheritance. |
| G.11| Use assertions to validate inputs when type checks aren't sufficient. |

*Note: Some coding conventions may have been overlooked during the writing of this document, so always refer to the existing codebase.*

## Thanks!
Thanks for being a part of the [Overload Technologies](https://github.com/Overload-Technologies) team!