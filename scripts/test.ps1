[CmdletBinding()]
param(
  [ValidateSet('lab00', 'lab01', 'lab02', 'all')] [string]$Lab = 'all',
  [switch]$WithSimulation
)

$ErrorActionPreference = 'Stop'
$root = Split-Path -Parent $PSScriptRoot
$labs = if ($Lab -eq 'all') { 'lab00', 'lab01', 'lab02' } else { @($Lab) }
foreach ($name in $labs) {
  $tests = if ($name -eq 'lab00') { Join-Path $root "labs/$name/tests" } else { Join-Path $root "$name/tests" }
  if (Test-Path -LiteralPath $tests) {
    Get-ChildItem -LiteralPath $tests -Filter '*Tests.ps1' | ForEach-Object { & $_.FullName }
  }
}
if ($WithSimulation) {
  if ($Lab -ne 'lab00') { throw 'Симуляционный каркас сейчас добавлен только для lab00/smoke.' }
  & (Join-Path $PSScriptRoot 'simulate.ps1') -Lab lab00 -Sketch smoke
}
Write-Host 'Host-проверки завершены.'
