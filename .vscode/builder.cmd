::
:: (c) 2020 dbj@dbj.org -- https://dbj.org/license_dbj
::
:: poor, windows dev, build script
::
:: args are positional, unnamed and mandatory
::
:: %1 -- workspace folder full path -- aka the root folder of this project
:: %2 -- exe base name without the ".exe" 
:: %3 -- CL or CLANG -- local Visual Studio 2019 installation has to include both
::                      you must run this script from VS x64 command prompt
::                      which anyway is implied as you will be running it from inside VS Code
::                      clang-cl.exe is x86 variery and its path is hard coded here, see bellow
:: %4 -- CXX_ON or CXX_OFF or KERNEL -- c++ exceptions on or off
::                            if off or KERNEL , try/throw/catch are forbidden in the language, by the compiler
::                            if off or KERNEL, RTTI is off to
::                            SEH is always here and intrinsic to cl.exe
::                            SEH code uses __try/__except/__finally
:: %5 -- RELEASE or DEBUG --  guess what is this for
::
:: each build attributes are
::
:: - build platform: win10 pro x64
:: - UNICODE
:: - static runtime lib 
:: 
:: as you might have noticed this is best called from VSCode tasks.json
::
:: {
::    "version": "2.0.0",
::    "tasks": [
::        {
::            "type": "shell",
::            "label": "CLANG_NO_EXCEPTIONS_RELEASE",
::            "command": "start ${workspaceFolder}\\.vscode\\builder.cmd",
::            "args": [ "${workspaceFolder}", "bench_clang_seh_release", "CLANG", "CXX_OFF", "RELEASE" ],
::            "detail": "calling builder.bat"
::        }
::    ]
:: }
::
:: ADVICE: do not even think changing batch scripts without using https://ss64.com
::

@echo off&title="dbj builder (c) 2020 by dbj@dbj.org -- https://dbj.org/license_dbj"
@cls
@Setlocal EnableDelayedExpansion

if [%1] == [] (
    @echo.
    @echo First argument has to be a workspace folder full path
    @echo.
    exit /b
)
set "workspaceFolder=%1"

if [%2] == [] (
    @echo.
    @echo Second argument has to be exe name, without the '.exe'
    @echo.
    exit /b
)
@set "exename=%2"

:: core requirement: building on WIN10 x64
:: must use from VS x64 command prompt
@set "cl_exe=cl.exe"

:: this is 32 bit clang-cl.exe full path on this machine
@set clang_cl="D:\PROD\programs\Microsoft Visual Studio\2019\Community\VC\Tools\Llvm\bin\clang-cl.exe"
:: thus it has to be given the x64 target as mandatory since we said the hard requirement
:: is to use this on WIN10 x64
@set clang_arch=" --target=x86_64-pc-windows-msvc"

:: common for any build
@set "args= /nologo" 
@set "args=%args% /Zc:wchar_t" 
@set "args=%args% /std:c++17" 
@set "args=%args% /DUNICODE" 
@set "args=%args% /D_UNICODE" 
:: build platform is WIN10
@set "args=%args% /DWINVER=0x0A00" 
@set "args=%args% /D_WIN32_WINNT=0x0A00" 

if [%3] == [CL] (
@set "compiler=%cl_exe%"
goto decide_on_exceptions
)

if [%3] == [CLANG] (
@set "compiler=%clang_cl%"
@set "args=%clang_arch% %args%"
goto decide_on_exceptions
)

@echo.
@echo Third argument has to be CL or CLANG
@echo.
@exit /b

:decide_on_exceptions
:: %4 -- CXX_ON or CXX_OFF -- c++ exceptions on or off
if [%4] == [CXX_ON] (
@set "args=%args% /GR" 
@set "args=%args% /D_HAS_EXCEPTIONS=1"      
@set "args=%args% /EHsc"      
goto :decide_on_debug_or_release
)

if [%4] == [CXX_OFF] (
:: no RTTI
@set "args=%args% /GR-" 
@set "args=%args% /D_HAS_EXCEPTIONS=0"
:: do we need to use /kernel      
goto :decide_on_debug_or_release
)
:: some users want to use the /kernel switch
if [%4] == [KERNEL] (
:: no RTTI
@set "args=%args% /GR-" 
:: vcruntime.h does _HAS_EXCEPTIONS=0 if this is used
@set "args=%args% /kernel"
goto :decide_on_debug_or_release
)

@echo.
@echo Fourth argument must be CXX_ON or CXX_OFF or KERNEL
@echo.
@exit /b

:decide_on_debug_or_release

if [%5] == [DEBUG] (
:: yes this is standard symbol for release builds 
@set "args=%args% /DDEBUG"
:: _DEBUG is intrinsic for Windows debug builds 
@set "args=%args% /_DEBUG" 
:: runtime lib is static debug
@set "args=%args% /MTd"
goto :build_the_thing
)

if [%5] == [RELEASE] (
:: yes this is standard symbol for release builds 
@set "args=%args% /DNDEBUG" 
:: runtime lib is static no-debug
@set "args=%args% /MT"
goto :build_the_thing
)
 
@echo.
@echo Fifth argument must be DEBUG or RELEASE
@echo.
@exit /b

:: -----------------------------------------------------------------------------
:build_the_thing

:: location of EASTL
@set "args=%args% /I%workspaceFolder%\EASTL2020CORE\include"
::
@set "args=%args% /Fe%workspaceFolder%/%exename%.exe" 
::  in each build in this project
@set files="%workspaceFolder%\program.cpp" 
::  
@set "files=%files% %workspaceFolder%\ubenches\*.cpp" 
@set "files=%files% %workspaceFolder%\ubenches\*.c" 
@set "files=%files% %workspaceFolder%\ubenches\allocators\*.cpp" 
@set "files=%files% %workspaceFolder%\ubenches\allocators\kalloc\*.c" 
@set "files=%files% %workspaceFolder%\ubenches\allocators\nvwa\*.cpp"
:: simply add the libs to the list of files
@set "files=%files% %workspaceFolder%\x64\release\*.lib"
::
@echo on
%compiler% %args% %files%
@echo off