@echo off
setlocal

set "LAB_ROOT=%~dp0"
set "REPO_ROOT=%LAB_ROOT%..\"
set "CODE_EXE=%LOCALAPPDATA%\Programs\Microsoft VS Code\Code.exe"
set "WORKSPACE=%REPO_ROOT%microprocessor-labs.code-workspace"
set "SOURCE=%LAB_ROOT%src\task4_lcd_keypad\task4_lcd_keypad.ino"
set "DIAGRAM=%LAB_ROOT%circuits\task4-lcd-keypad\diagram.json"

if not exist "%CODE_EXE%" (
  echo Visual Studio Code was not found:
  echo %CODE_EXE%
  pause
  exit /b 1
)

if not exist "%DIAGRAM%" (
  echo Wokwi diagram was not found:
  echo %DIAGRAM%
  pause
  exit /b 1
)

start "" "%CODE_EXE%" --new-window "%WORKSPACE%" "%SOURCE%" "%DIAGRAM%"
endlocal
