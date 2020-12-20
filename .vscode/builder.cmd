@ECHO OFF&TITLE="dbj builder (c) 2020 by dbj@dbj.org -- https://dbj.org/license_dbj"
@CLS
@Setlocal EnableDelayedExpansion
@REM SET prompt to '-> ' for debugging 
@PROMPT -$G 

@REM
@REM (c) 2020 dbj@dbj.org -- https://dbj.org/license_dbj
@REM
@REM poor, windows dev, build script
@REM
@REM args are positional, unnamed and mandatory
@REM
@REM %1 -- workspace folder full path -- aka the root folder of this project
@REM %2 -- exe base name without the ".exe" 
@REM %3 -- CL or CLANG -- local Visual Studio 2019 installation has to include both
@REM                      you must run this script from VS x64 command prompt
@REM                      which anyway is implied as you will be running it from inside VS Code
@REM                      clang-cl.exe is x86 variery and its path is hard coded here, see bellow
@REM %4 -- CXX_ON or CXX_OFF or KERNEL -- c++ exceptions on or off
@REM                            IF off or KERNEL , try/throw/catch are forbidden in the language, by the compiler
@REM                            IF off or KERNEL, RTTI is off to
@REM                            SEH is always here and intrinsic to cl.exe
@REM                            SEH code uses __try/__except/__finally
@REM %5 -- RELEASE or DEBUG --  guess what is this for
@REM
@REM common build attributes are
@REM
@REM - build platform: win10 pro x64
@REM - UNICODE
@REM - static runtime lib 
@REM 
@REM as you might have noticed this is best called from VSCode tasks.json
@REM
@REM {
@REM    "version": "2.0.0",
@REM    "tasks": [
@REM        {
@REM            "type": "shell",
@REM            "label": "CLANG_NO_EXCEPTIONS_RELEASE",
@REM            "command": "start ${workspaceFolder}\\.vscode\\builder.cmd",
@REM            "args": [ "${workspaceFolder}", "bench_clang_seh_release", "CLANG", "CXX_OFF", "RELEASE" ],
@REM            "detail": "calling builder.bat"
@REM        }
@REM    ]
@REM }
@REM
@REM ADVICE: do not even think changing batch scripts without using https://ss64.com
@REM

IF [%1] EQU [] (
    @ECHO.
    @ECHO First argument has to be a workspace folder full path
    @ECHO.
    exit /b
)
SET "workspaceFolder=%1"

IF [%2] EQU [] (
    @ECHO.
    @ECHO Second argument has to be exe name, without the '.exe'
    @ECHO.
    exit /b
)
SET "exename=%2"

@REM core requirement: building on WIN10 x64
@REM must use from VS x64 command prompt
SET cl_exe=cl.exe

@REM this is 32 bit clang-cl.exe full path on this machine
@REM on the WS01 
@REM 32 bit
SET clang_cl="D:\PROD\programs\Microsoft Visual Studio\2019\Community\VC\Tools\Llvm\bin\clang-cl.exe"
@REM on me laptop
: @SET clang_cl="C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Tools\Llvm\bin\clang-cl.exe"
@REM thus it has to be given the x64 target as mandatory since we said the hard requirement
@REM is to use this on WIN10 x64
SET clang_arch=--target=x86_64-pc-windows-msvc

@REM common for any build
@REM build platform is WIN10
SET args= /nologo /Zc:wchar_t /std:c++17 /DUNICODE /D_UNICODE /DWINVER=0x0A00 /D_WIN32_WINNT=0x0A00 

IF [%3] EQU [CL] (
SET compiler=%cl_exe%
GOTO decide_on_exceptions
)

IF [%3] EQU [CLANG] (
SET compiler=%clang_cl%
SET args=%clang_arch% %args%
GOTO decide_on_exceptions
)

@ECHO.
@ECHO Third argument has to be CL or CLANG
@ECHO.
@exit /b

:decide_on_exceptions
@REM %4 -- CXX_ON or CXX_OFF -- c++ exceptions on or off
IF [%4] EQU [CXX_ON] (
SET args=%args% /GR /D_HAS_EXCEPTIONS=1 /EHsc      
GOTO decide_on_debug_or_release
)

@REM /GR- means no RTTI
IF [%4] EQU [CXX_OFF] (
SET args=%args% /GR- /D_HAS_EXCEPTIONS=0
GOTO decide_on_debug_or_release
)
@REM some users want to use the /kernel switch
IF [%4] EQU [KERNEL] (
SET args=%args% /GR- /kernel
GOTO decide_on_debug_or_release
)

@ECHO.
@ECHO Fourth argument must be CXX_ON or CXX_OFF or KERNEL
@ECHO.
@exit /b

:decide_on_debug_or_release

@REM _DEBUG is intrinsic for Windows debug builds 
IF [%5] EQU [DEBUG] (
SET args=%args% /DDEBUG /_DEBUG /MTd
GOTO build_the_thing
)

@REM NDEBUG is standard symbol for release builds 
IF [%5] EQU [RELEASE] (
SET args=%args% /DNDEBUG /MT
GOTO build_the_thing
)
 
@ECHO.
@ECHO Fifth argument must be DEBUG or RELEASE
@ECHO.
@exit /b

@REM -----------------------------------------------------------------------------
:build_the_thing

@REM linker switches
@REM here we are building console apps 
SET "linker=/link /SUBSYSTEM:CONSOLE"

@REM location of EASTL
SET args=%args% /I"%workspaceFolder%\EASTL2020CORE\include"
@REM
SET args=%args% /Fe"%workspaceFolder%\%exename%.exe" 
@REM  in each build in this project
SET files="%workspaceFolder%\program.cpp" 
@REM  
SET "files=%files% %workspaceFolder%\ubenches\*.cpp" 
SET "files=%files% %workspaceFolder%\ubenches\*.c" 
SET "files=%files% %workspaceFolder%\ubenches\allocators\*.cpp" 
SET "files=%files% %workspaceFolder%\ubenches\allocators\kalloc\*.c" 
SET "files=%files% %workspaceFolder%\ubenches\allocators\nvwa\*.cpp"
@REM simply add the libs to the list of files
SET "files=%files% %workspaceFolder%\x64\release\*.lib"

@REM GOTO DEBUG_DISPLAY

@ECHO on
%compiler% %args% %files% %linker%
@ECHO off
GOTO:EOF

:DEBUG_DISPLAY
@REM
@REM for debug just show what is composed to be used
@ECHO.
@ECHO compiler :  %compiler%
@ECHO.
@ECHO args :  %args%
@ECHO.
@ECHO files :  %files%
@ECHO.
@ECHO linker :  %linker%
@ECHO.
GOTO:EOF

@ECHO.
@ECHO Done :  %workspaceFolder%\%exename%.exe
@ECHO.
