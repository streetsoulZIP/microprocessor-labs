@echo off
setlocal

set "LAB_ROOT=%~dp0"
set "REPO_ROOT=%LAB_ROOT%..\"
set "SKETCH=task06_brightness_lock"
set "SOURCE=%LAB_ROOT%src\%SKETCH%\%SKETCH%.ino"
set "CIRCUIT=%LAB_ROOT%circuits\%SKETCH%"
set "DIAGRAM=%CIRCUIT%\diagram.json"
set "BUILD_DIR=%REPO_ROOT%build\lab00\%SKETCH%"
set "CODE_EXE=%LOCALAPPDATA%\Programs\Microsoft VS Code\Code.exe"

where arduino-cli >nul 2>nul
if errorlevel 1 (
  echo Arduino CLI was not found in PATH.
  pause
  exit /b 1
)

if not exist "%CODE_EXE%" (
  echo Visual Studio Code was not found:
  echo %CODE_EXE%
  pause
  exit /b 1
)

cd /d "%REPO_ROOT%"
echo Building lab00 task 6...
arduino-cli compile --fqbn arduino:avr:uno --output-dir "%BUILD_DIR%" "%LAB_ROOT%src\%SKETCH%"
if errorlevel 1 (
  echo Build failed.
  pause
  exit /b 1
)

start "" "%CODE_EXE%" --new-window "%CIRCUIT%" "%SOURCE%" "%DIAGRAM%"
timeout /t 6 /nobreak >nul
start "" "vscode://command/wokwi-vscode.start"

endlocal
