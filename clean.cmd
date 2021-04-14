::
:: (c) 2020 DEV 03 dbj@dbj.org
:: windows only script
:: any argument will make exe's and pdb's  
:: and Visual Studio artefacts deleted, too
::
@echo on
@setlocal
FOR /F "tokens=*" %%G IN ('dir /b /s *.obj') DO del /q %%G
FOR /F "tokens=*" %%G IN ('dir /b /s *.ilk') DO del /q %%G

:: remove the VStudio artefacts
FOR /F "tokens=*" %%G IN ('dir /b /s *.filters') DO del /q %%G
FOR /F "tokens=*" %%G IN ('dir /b /s *.user') DO del /q %%G

@IF [%1] == [] GOTO script_exit

:: remove the VStudio folders artefacts
FOR /F "tokens=*" %%G IN ('dir /b /s x64') DO rd /s /q %%G
FOR /F "tokens=*" %%G IN ('dir /a:h /b /s .vs') DO rd /s /q %%G

:script_exit