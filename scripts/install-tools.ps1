[CmdletBinding()]
param([switch]$ArduinoOnly, [switch]$WokwiOnly)

$ErrorActionPreference = 'Stop'
if (-not $WokwiOnly -and -not (Get-Command arduino-cli -ErrorAction SilentlyContinue)) {
  winget install --id ArduinoSA.CLI --exact --source winget --accept-package-agreements --accept-source-agreements
}
if (-not $ArduinoOnly -and -not (Get-Command wokwi-cli -ErrorAction SilentlyContinue)) {
  # Official Wokwi installer: https://docs.wokwi.com/wokwi-ci/cli-installation
  Invoke-RestMethod 'https://wokwi.com/ci/install.ps1' | Invoke-Expression
}
if (-not $WokwiOnly) {
  $cli = (Get-Command arduino-cli -ErrorAction SilentlyContinue).Source
  if ($cli) { & $cli core update-index; & $cli core install arduino:avr }
}
