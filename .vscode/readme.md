### Win10 + Visual Studio 2019 + clang-cl + VS Code

Install Visual Studio 2019 with clang. Now open cmd.exe and goto clang bin directory.
Execute `clang-cl --version`. On my `W10` machine here that displays:

```
C:\Microsoft Visual Studio\2019\Community\VC\Tools\Llvm\bin>clang-cl --version
clang version 10.0.0
Target: i686-pc-windows-msvc
Thread model: posix
InstalledDir: C:\Microsoft Visual Studio\2019\Community\VC\Tools\Llvm\bin
```

that `Target` means clang-cl indeed "just works", but only for 32 bit command line builds


Thus it will not link if you start the VS Code from Visual Studio 64 bit command line prompt, aka `vcvars64.bat`. 

Ditto:

```
--target=x86_64-pc-windows-msvc
```

is a necessary switch for clang-cl 64 bit builds. 

> Without that switch linker will not link after clang-cl succesfully compiles in 64 bit mode.