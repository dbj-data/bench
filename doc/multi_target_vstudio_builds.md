
# Multi Target Builds

Visual Studio 2019 is used. No CMake.

## Directory structure

For each project all building artefacts are under the build folder. Thus they are decoupled from the code.

Publich headers are "in the front", in the library top folder.

When building the hosting app, you reference these projects, VStudio picks the right lib files. 

This is one-to-one. Hosting app is built following the same paradigm as in here. One hoting app build link one build from here.

## Main build attributes

1. Compilers: clang-cl  or cl
   1. Why: clang-cl because it allows for powerfull mix and match of C11/C17 code with C++ code in one cpp file.
2. C++ exceptions or no C++ exceptions
   1. Why: no C++ exceptions, because executables are smaller and faster. Also. exceptions are not convieved for local error handling. You calling `fopen()` will never involve exceptions. 
   2. no this can not be avoided by using std:: lib always
      1. std lib throws very few exceptions 
      2. MS STL can be build and does run very fast without C++ exceptions 
         1. It uses SEH in that case

## Common to all builds

>
> Fact: On Windows, SEH is always present. It is intrinsic to Windows.
>
> Static run-time lib is always used
> 

| switch | comment
|--------|-------------
| /d2FH4 | ["Modi Mo"](https://devblogs.microsoft.com/cppblog/making-cpp-exception-handling-smaller-x64/) <br/> clang does not understand <br/> this is in-built, we use it to emphasise the point
| /permissive- |
| /std:c++17 | clang does not understand
| /nologo | 
| /Zc:wchar_t  | wchar_t is inbuilt
| /std:c++17 | clang does not understand
| /DUNICODE /D_UNICODE |
| /DWINVER=0x0A00 /D_WIN32_WINNT=0x0A00 | build platform

### /LTCG aka "Link Time Code Generation" 

Must be set to "No" for clang-cl builds

### Only x64

32 bit builds are not built at all.

### EASTL and C++ exceptions

```
EASTL_EXCEPTIONS_ENABLED
```

EASTL does not enable exceptions by default if the compiler
has exceptions enabled. To enable EASTL_EXCEPTIONS_ENABLED you need to
manually set it to 1. 

Thus most of the time EASTL is with no C++ exceptions. It uses it's own assert.cpp.

When EASTL has C++ exceptions enabled it even used std::exception and offsprings. This is how rarely, if ever, anybody uses EASTL in projects where C++ exceptions are enabled.

## The core of the stunt

"Shared code"  (VStudio project type) (for example) "dbj-eastl-code" collects all the code.

All static lib projets reference that one. That is the only place where we add/remove/update the code. That is propagated to the building projects.

Lib projects have no compilation units. Beside pch.cpp.

> Never enter any code into pch.h or pch.cpp, they are building artefacts only.

The hosting app is built by the same principles. Each hosting app build type references matching lib from here.

## Naming is important

Extremely important. There is no tool to make this painless, just the rule:
```xml
<"base name"> + <"clang" | ""> + <"no cppx" | "cppx >
```

- if no "clang" in the name , cl is used
- "cppx" stands for cpp exceptions
  - SEH is intrinsic to Windows and is always present. 
    - One can use it or ignore it.

## What's the big deal with C++ exceptions?

   1. Why: no C++ exceptions, because executables are smaller and faster. 
   2. exceptions are not concieved for local error handling. You calling `fopen()` will never involve exceptions. 
      1. no this can not be avoided by using std:: lib always
      2. std lib throws very few exceptions 
      3. MS STL can be built and does run very fast without C++ exceptions 

 Ben Craig, P1886R0 Error speed benchmarking, http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2019/p1886r0.html

 Ben Craig, Error size benchmarking: Redux , http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2019/p1640r1.html

---
> (c) 2019-2020 by dbj.org   -- https://dbj.org/license_dbj/