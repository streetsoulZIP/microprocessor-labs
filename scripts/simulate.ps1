[CmdletBinding()]
param(
  [ValidateSet('lab00', 'lab01', 'lab02')] [string]$Lab = 'lab00',
  [string]$Sketch = 'smoke',
  [int]$TimeoutMs = 5000
)

$ErrorActionPreference = 'Stop'
$root = Split-Path -Parent $PSScriptRoot
$project = Join-Path $root "labs/$Lab/$Sketch"
$cli = (Get-Command wokwi-cli -ErrorAction SilentlyContinue).Source
if (-not $cli) {
  $bundledCli = Join-Path $env:USERPROFILE '.wokwi\bin\wokwi-cli.exe'
  if (Test-Path -LiteralPath $bundledCli) { $cli = $bundledCli }
}
if (-not $cli) {
  throw 'Wokwi CLI не найден. Установите его: .\scripts\install-tools.ps1 -WokwiOnly. Для браузера: https://wokwi.com/projects/new/arduino-uno'
}
if (-not $env:WOKWI_CLI_TOKEN) {
  # Codex/terminals started before setting a User variable do not inherit it.
  # Copy only into this process; do not persist or display the secret.
  $userToken = [Environment]::GetEnvironmentVariable('WOKWI_CLI_TOKEN', 'User')
  if ($userToken) { $env:WOKWI_CLI_TOKEN = $userToken }
}
if (-not $env:WOKWI_CLI_TOKEN) {
  throw 'Не задан WOKWI_CLI_TOKEN. Войдите в Wokwi CI, сохраните токен в переменной окружения пользователя и откройте новый PowerShell. Токен не записывается в проект.'
}
& (Join-Path $PSScriptRoot 'build.ps1') -Lab $Lab -Sketch $Sketch
$results = Join-Path $root "labs/$Lab/results"
New-Item -ItemType Directory -Force -Path $results | Out-Null
$serial = Join-Path $results "$Sketch-serial.log"
Push-Location $project
try {
  # CLI reads diagram.json/wokwi.toml in the project directory. --timeout prevents quota waste.
  $scenario = Join-Path $project 'test.yaml'
  if (-not (Test-Path -LiteralPath $scenario)) { $scenario = Join-Path $project 'smoke.test.yaml' }
  if (Test-Path -LiteralPath $scenario) {
    & $cli . --timeout $TimeoutMs --serial-log-file $serial --scenario $scenario
  } else {
    & $cli . --timeout $TimeoutMs --serial-log-file $serial
  }
  if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }
} finally { Pop-Location }
Write-Host "Serial log: $serial"
