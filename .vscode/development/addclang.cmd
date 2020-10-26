
@echo on
@cls

::
:: (c) 2020 by dbj@dbj.org
:: LICENSE_DBJ -- https://dbj.org/license_dbj
::

if "%VSCMD_VER%"=="" (
    @echo please start %0 after starting Visual Studio 2019 cmd line script first!
    goto exit
)

@echo.
@echo VSCMD_ARG_app_plat = %VSCMD_ARG_app_plat%
@echo VSCMD_ARG_HOST_ARCH = %VSCMD_ARG_HOST_ARCH%
@echo VSCMD_ARG_TGT_ARCH = %VSCMD_ARG_TGT_ARCH%
@echo VSCMD_VER = %VSCMD_VER%
@echo VCINSTALLDIR = %VCINSTALLDIR%
@echo.

SET "CLANGCL_BIN="
::
:: Tools\Llvm\bin
:: SETX CLANGCL_BIN "%CLANGCL_BIN%"
if  "%VSCMD_ARG_TGT_ARCH%"=="x86" (
    SET CLANGCL_BIN="%VCINSTALLDIR%Tools\Llvm\bin\clang-cl.exe"
)

::
:: Tools\Llvm\x64\bin
:: SETX CLANGCL_BIN "%CLANGCL_BIN%"
if  "%VSCMD_ARG_TGT_ARCH%"=="x64" (
    SET CLANGCL_BIN="%VCINSTALLDIR%Tools\Llvm\x64\bin\clang-cl.exe"
)

if %CLANGCL_BIN%=="" (
    @echo Unknown architecture %VSCMD_ARG_TGT_ARCH% ?
    goto exit
)

::    SET  "PATH=%CLANGCL_BIN%;%PATH%"
::    SETX PATH "%CLANGCL_BIN%";%PATH%

    @echo.
    @echo CLANGCL_BIN : %CLANGCL_BIN% 
    @echo.
    @echo.
    @echo -----------------------------------------------------------------------

    %CLANGCL_BIN%  --version

    @echo.
    @echo -----------------------------------------------------------------------
    @cl 
    @echo.
    @echo -----------------------------------------------------------------------
    @echo Please make sure cl.exe and clang-cl.exe target architectures do  match
:exit
    @echo.
    @echo %0 -- (c) 2020 by dbj@dbj.org
    @echo.



