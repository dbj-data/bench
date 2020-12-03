::
:: (c) 2020 DEV 03 dbj@dbj.org
:: windows only script
:: any argument will make exe's and pdb's be 
:: and Visual Stuydio artefacts deleted, too
::
@echo off
@del /s *.obj>nul
@del /s *.ilk>nul
@IF [%1] == [] GOTO script_exit
@del /s *.exe>nul
@del /s *.pdb>nul

:: remove the VStudio artefacts
@del /s *.filters>nul
@del /s *.user>nul

FOR /F "tokens=*" %%G IN ('dir /b /s x64') DO @rd /s /q %%G
FOR /F "tokens=*" %%G IN ('dir /b /s .vs') DO @rd /s /q %%G

:script_exit