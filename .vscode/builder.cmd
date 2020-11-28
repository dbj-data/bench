
:: "CLANG_64bit_SEH_RELEASE_BUILD"
@echo off&title=dbj builder
@cls
@Setlocal EnableDelayedExpansion

if [%1] == [] (
    @echo.
    @echo First argument has to be a build tag. Build tags are
    @echo.
    @echo  CLANG_64bit_SEH_RELEASE
    @echo.
    exit /b
)
set "build_tag=%1"

if [%2] == [] (
    @echo.
    @echo Second argument has to be a workspace folder full path
    @echo.
    exit /b
)
set "workspaceFolder=%2"

if [%3] == [] (
    @echo.
    @echo Third argument has to be a exe name, without the '.exe'
    @echo.
    exit /b
)
set "exename=%3"

:: this is 32 bit clang-cl.exe
@set command="D:\PROD\programs\Microsoft Visual Studio\2019\Community\VC\Tools\Llvm\bin\clang-cl.exe"
:: thus it has to be given the x64 target
@set args="--target=x86_64-pc-windows-msvc"

@set "args=%args% /I%workspaceFolder%\EASTL2020CORE\include"
@set "args=%args% /nologo" 
@set "args=%args% /Zc:wchar_t" 
@set "args=%args% /std:c++17" 
@set "args=%args% /GR-" 
@set "args=%args% /D_HAS_EXCEPTIONS=0" 
@set "args=%args% /DUNICODE" 
@set "args=%args% /D_UNICODE" 
@set "args=%args% /DDBJ_USE_UBENCH" 
@set "args=%args% /DNDEBUG" 
@set "args=%args% /DDBJ_FWK_DISPLAY_INFO" 
@set "args=%args% /DWINVER=0x0A00" 
@set "args=%args% /D_WIN32_WINNT=0x0A00" 
@set "args=%args% /MT" 
@set "args=%args% /Fe%workspaceFolder%/%exename%.exe" 

:: @set "files=%workspaceFolder%/dbj-fwk/main.cpp" 
@set files="%workspaceFolder%\program.cpp" 
:: @set "files=%files% %workspaceFolder%\units\*.c" 
:: @set "files=%files% %workspaceFolder%\units\*.cpp" 
:: @set "files=%files% %workspaceFolder%\EASTL2020CORE\source\*.cpp" 
@set "files=%files% %workspaceFolder%\ubenches\*.cpp" 
@set "files=%files% %workspaceFolder%\ubenches\*.c" 
@set "files=%files% %workspaceFolder%\ubenches\allocators\*.cpp" 
@set "files=%files% %workspaceFolder%\ubenches\allocators\kalloc\*.c" 
@set "files=%files% %workspaceFolder%\ubenches\allocators\nvwa\*.cpp"

:: simply add the libs to the list of files
@set "files=%files% %workspaceFolder%\x64\release\*.lib"

@echo on
:: do it
%command% %args% %files%
@echo of