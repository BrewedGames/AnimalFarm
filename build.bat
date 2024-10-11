@echo off

if "%1"=="-2019" (
    premake5 vs2019
) else if "%1"=="-2022" (
    premake5 vs2022
) else (
    echo Invalid flag. Please use either -2019 or -2022.
)