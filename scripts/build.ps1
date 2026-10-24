[CmdletBinding()]
param(
  [ValidateSet('lab00', 'lab01', 'lab02')] [string]$Lab = 'lab00',
  [string]$Sketch = 'smoke'
)

$ErrorActionPreference = 'Stop'
$root = Split-Path -Parent $PSScriptRoot
$sketchDir = if ($Lab -eq 'lab00') { Join-Path $root "labs/$Lab/$Sketch" } else { Join-Path $root "$Lab/src/$Sketch" }
$sketchFile = Join-Path $sketchDir "$Sketch.ino"
if (-not (Test-Path -LiteralPath $sketchFile)) { throw "Скетч не найден: $sketchFile" }
$cli = (Get-Command arduino-cli -ErrorAction SilentlyContinue).Source
if (-not $cli) { $cli = 'C:\Program Files\Arduino CLI\arduino-cli.exe' }
if (-not (Test-Path -LiteralPath $cli)) {
  throw 'Arduino CLI не найден. Выполните .\scripts\install-tools.ps1 -ArduinoOnly.'
}
$out = Join-Path $root "build/$Lab/$Sketch"
$logDir = Join-Path $root 'artifacts/build-logs'
New-Item -ItemType Directory -Force -Path $out, $logDir | Out-Null
$log = Join-Path $logDir "$Lab-$Sketch-$(Get-Date -Format yyyyMMdd-HHmmss).log"
& $cli compile --fqbn arduino:avr:uno --output-dir $out $sketchDir 2>&1 | Tee-Object -FilePath $log
if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }
Write-Host "Готово: $out"
Write-Host "Лог: $log"
