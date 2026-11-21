@echo off
setlocal

set "LAB_ROOT=%~dp0"
set "REPO_ROOT=%LAB_ROOT%..\"
set "SKETCH=%~1"
set "SOURCE=%LAB_ROOT%src\%SKETCH%\%SKETCH%.ino"
set "CIRCUIT=%LAB_ROOT%circuits\%SKETCH%"
set "DIAGRAM=%CIRCUIT%\diagram.json"
set "BUILD_DIR=%REPO_ROOT%build\lab04\%SKETCH%"
set "CODE_EXE=%LOCALAPPDATA%\Programs\Microsoft VS Code\Code.exe"

if "%SKETCH%"=="" (
  echo A task name is required.
  pause
  exit /b 1
)

where arduino-cli >nul 2>nul
if errorlevel 1 (
  echo Arduino CLI was not found in PATH.
  pause
  exit /b 1
)

if not exist "%SOURCE%" (
  echo Source was not found: %SOURCE%
  pause
  exit /b 1
)

if not exist "%CODE_EXE%" (
  echo Visual Studio Code was not found: %CODE_EXE%
  pause
  exit /b 1
)

cd /d "%REPO_ROOT%"
echo Building lab04 %SKETCH%...
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
