$root = Split-Path -Parent $PSScriptRoot
foreach ($variant in 'switch_fsm','table_fsm') {
  $text = Get-Content -Raw (Join-Path $root "src\$variant\$variant.ino")
  foreach ($needle in 'millis\(','DEBOUNCE\s*=\s*50','PED_QUEUED','EMERGENCY_ON','NIGHT_ON','TIMER_PED','INPUT_PULLUP') {
    if ($text -notmatch $needle) { throw "${variant}: отсутствует $needle" }
  }
}
$table = Get-Content -Raw (Join-Path $root 'src\table_fsm\table_fsm.ino')
if ($table -notmatch 'Handler table\[STATE_COUNT\]\[EVENT_COUNT\]') { throw 'table_fsm: отсутствует двумерная таблица handlers' }
Write-Host 'lab02 host source checks: OK'
