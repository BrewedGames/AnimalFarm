@echo off

set BINDIR="%CD%/bin/Debug/"
IF not exist %BINDIR% (mkdir %BINDIR%)

if "%1"=="-2019" (
    %CD%/installer/StarStruck_Installer.exe
    build -2019
    copy "C:\StarStruck\lua\bin\*" "%CD%/bin/Debug/"
    copy "C:\StarStruck\OpenGL\bin\*" "%CD%/bin/Debug/"
    copy "C:\StarStruck\SDL\bin\*" "%CD%/bin/Debug/"
) else if "%1"=="-2022" (
    %CD%/installer/StarStruck_Installer.exe
    build -2022
    copy "C:\StarStruck\lua\bin\*" "%CD%/bin/Debug/"
    copy "C:\StarStruck\OpenGL\bin\*" "%CD%/bin/Debug/"
    copy "C:\StarStruck\SDL\bin\*" "%CD%/bin/Debug/"

) else (
    echo Invalid flag. Please use either -2019 or -2022.
)