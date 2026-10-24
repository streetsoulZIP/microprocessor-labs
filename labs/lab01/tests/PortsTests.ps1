$source = Get-Content -Raw (Join-Path $PSScriptRoot '..\ports\ports.ino')
foreach ($required in 'DDRD &= ~BUTTON_MASK','PORTD |= BUTTON_MASK','DDRB |= LED_MASK','PIND','__builtin_ctz','bitSet\(PORTB','SAMPLE_MS = 200','DEBOUNCE_MS = 40','printSetPositions','POS=\[') {
  if ($source -notmatch $required) { throw "Требуемая конструкция не найдена: $required" }
}
Write-Host 'lab01 host source checks: OK'
