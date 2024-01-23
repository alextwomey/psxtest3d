@echo off
set /p file="Enter XML file to make: "
echo making ".\%file%"
.\mkpsxiso.exe .\%file%
pause
