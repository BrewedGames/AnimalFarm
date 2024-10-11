@echo off
rem Open Developer Command Prompt for VS 2022
call "C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\Tools\VsDevCmd.bat"

rem Change directory to the folder where the .bat file is located
cd /d "%~dp0"

rem Open the folder in VS Code
code .
