@echo off
set "ROOT=%~dp0.."
powershell -NoProfile -ExecutionPolicy Bypass -File "%ROOT%\scripts\build.ps1" -Lab lab02 -Sketch switch_fsm
if errorlevel 1 pause & exit /b 1
code "%~dp0circuits\switch-case"
