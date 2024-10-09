@echo off
call make.bat
echo done building

if not exist main.exe (
    echo where is main.exe?
)

main.exe