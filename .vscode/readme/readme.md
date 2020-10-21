<h1> clang + VS Code, on Windows 10</h1>

- [1.0.1. Difficult path](#101-difficult-path)
- [1.0.2. The right path](#102-the-right-path)

You want (or need) to use VS Code to build your C/C++ projects on your machine using clang that comes packaged with Visual Stidio 2019. You have installed all the required extensions and now you are ready to use clang and compile. 

You have installed Visual Studio 2019 with clang. 

From Visual Studio, clang build "just works". But you want to use VS Code because you are in total control of the numerous, arcane, ridiculous and beautifull command line switches of the cl.exe compiler and its llvm specialy prepared `cl.exe` alter ego: `clang-cl.exe`. 

Now you use one of the VS installed "Command Prompt" icons on your start menu to open the cmd.exe and execute the `vcvarsall.bat` in a proper way. All done for you.

![vs cli icons](vs2019_start_the_cli.jpg)

You know you need to use `clang-cl.exe` as a llvm version of the `cl.exe`. You realize it is not on the path so you copy your default building configuration that VS Code has generated for you, and you just add the full path to it in your VS Code `taks.json`. Thus the critical line is now this:

(elipsis aka `...` is the root of your VS2019 installation)

```cpp
"command": "...\\Microsoft Visual Studio\\2019\\Community\\VC\\Tools\\Llvm\\bin\\clang-cl.exe",
```

And lo and behold! That works and you compile using `clang-cl`. 

But alas, the linker refuses to link. For me on my machine the message was this:

```
msvcrtd.lib(chkstk.obj) : fatal error LNK1112: module machine type 'x64' conflicts with target machine type 'x86'
clang-cl: error: linker command failed with exit code 1112 (use -v to see invocation)
The terminal process "C:\WINDOWS\System32\cmd.exe /d /c "D:\PROD\programs\Microsoft Visual Studio\2019\Community\VC\Tools\Llvm\bin\clang-cl.exe" /ID:\DEVL\serverside.systems\bench\EASTL2020CORE\include /std:c++17 /DUNICODE /D_UNICODE /D_DEBUG /Zc:wchar_t /Zi /MDd /FeD:\DEVL\serverside.systems\bench/bench_dbg.exe D:\DEVL\serverside.systems\bench/fwk/main.cpp D:\DEVL\serverside.systems\bench/program.cpp D:\DEVL\serverside.systems\bench\units\*.c D:\DEVL\serverside.systems\bench\units\*.cpp D:\DEVL\serverside.systems\bench\EASTL2020CORE\source\*.cpp" terminated with exit code: 1112.
```

Admitedly a message from hell. Perhaps translated into human it might be: 

"Back off boy! You are in the real man developers teritory. Leave while you can."

"But wait... stay!". You hear the voice. "Trust me, I will solve this for you. Let me take you down this..."

### 1.0.1. Difficult path

Step One. Click on that VS command line icon again: 

![vs cli icons](vs2019_start_the_cli.jpg)


Now, goto the clang bin directory. Execute `clang-cl --version`. On my `W10` machine that displays:

```
C:\Microsoft Visual Studio\2019\Community\VC\Tools\Llvm\bin>clang-cl --version
clang version 10.0.0
Target: i686-pc-windows-msvc
Thread model: posix
InstalledDir: C:\Microsoft Visual Studio\2019\Community\VC\Tools\Llvm\bin
```

Aha! That `Target` means clang-cl indeed "just works", but only for 32 bit command line builds. Dont ask, just trust me. Thus it will not link if you start the VS Code from Visual Studio 64 bit command line prompt, opened after `vcvars64.bat` is executed. 

Now, I have to tell you:

```
--target=x86_64-pc-windows-msvc
```

is a necessary switch for clang-cl 64 bit builds. Again, don't ask.

> Without that switch linker will not link after clang-cl succesfully compiles in 64 bit mode.

And now you clang build compiles and links. You are in the business.

Job done?

### 1.0.2. The right path

Is short and simple. You have gone to the wrong bin directory in the step one. Taken by me. 

> The whole point of the excersize is to match the right clang-cl.exe to x86 or x64 builds. 
 
Thus, after a long "devl night", it is a sunny morning, and first shoot of caffeine kicks in. And looking into:

`...\Microsoft Visual Studio\2019\Community\VC\Tools\Llvm\`

You spot the `x64` and you spot the `bin` under it. 

Yes! That is where the clang-cl for 64 bit builds is. So the above path was wrong, and we have found the way out from it. Although it is a bit of a moot point why we went down that path in the first place. One could just use the clang-cl.exe build for x64 and all will be fine.

```
...\Microsoft Visual Studio\2019\Community\VC\Tools\Llvm\x64\bin>clang-cl --version
clang version 10.0.0
Target: x86_64-pc-windows-msvc
Thread model: posix
InstalledDir: ...\Microsoft Visual Studio\2019\Community\VC\Tools\Llvm\x64\bin
```

Spot the `Target`.

Where the elipsis aka `...` is the root of your instalation of the Visual Studio 2019 that includes CLANG. As you requested while installing the said VS2019.

So. The right command in yuor tasks.json is now: (replace `...` with the root of your installation )

```cpp
"command": "...\\Microsoft Visual Studio\\2019\\Community\\VC\\Tools\\Llvm\\x64\\bin\\clang-cl.exe",
```

That is for the 64 bit clang-cl builds only.
