# Лабораторные по программированию микропроцессоров

Репозиторий содержит выполненные лабораторные работы для **Arduino Uno (ATmega328P, 16 МГц)**. Проекты собираются Arduino CLI и имеют интерактивные схемы Wokwi. Работа выполнялась без физической платы; в доказательствах отдельно отмечены реальные Wokwi-симуляции, сборки и статические проверки.

| Лабораторная | Тема | Отчёт | Реализация | Видео |
|---|---|---|---|---|
| 01 | Порты микроконтроллера и биты | [PDF](labs/lab01/reports/lab01-report.pdf) | [скетч](labs/lab01/ports/ports.ino) | [Wokwi](labs/lab01/results/lab01-wokwi-visual-demo-2026-10-10.mp4) |
| 02 | Конечный автомат двумя методами | [PDF](labs/lab02/reports/lab02-report.pdf) | [switch](labs/lab02/switch_fsm/switch_fsm.ino), [table](labs/lab02/table_fsm/table_fsm.ino) | [switch](labs/lab02/results/switch_demo.mp4), [table](labs/lab02/results/table_demo.mp4) |

Редактируемые исходники отчётов находятся рядом с PDF в формате DOCX. Скрипт [reports/build_academic_reports.py](reports/build_academic_reports.py) повторно создаёт обе пары PDF/DOCX из проверенных данных.

## Быстрый запуск

Откройте PowerShell в корне репозитория и выполните:

```powershell
Set-ExecutionPolicy -Scope Process -ExecutionPolicy Bypass
.\scripts\build.ps1 -Lab lab01 -Sketch ports
.\scripts\build.ps1 -Lab lab02 -Sketch switch_fsm
.\scripts\build.ps1 -Lab lab02 -Sketch table_fsm
.\scripts\test.ps1 -Lab lab01
.\scripts\test.ps1 -Lab lab02
```

Для визуального запуска в VS Code используйте `labs/lab02/open-switch-wokwi.cmd` или `labs/lab02/open-table-wokwi.cmd`; для lab01 откройте `labs/lab01/ports`. Затем выполните **Wokwi: Start Simulator**. Расширение Wokwi для VS Code может требовать собственную лицензию; бесплатная браузерная Wokwi-версия остаётся альтернативой.

## Подтверждённые результаты

- Lab01: 2712 Б Flash и 199 Б SRAM; Wokwi подтвердил одинарные/множественные нажатия, антидребезг и период 200 мс.
- Lab02 switch-case: 4400 Б Flash и 422 Б SRAM.
- Lab02 table-driven: 4460 Б Flash и 434 Б SRAM.
- У обоих вариантов lab02 успешно прошли реальные Wokwi-сценарии полного нормального цикла, аварийного и ночного режимов. Логи пешеходного сценария содержат ожидаемый результат, но его запускатель завершился `timeout 42` после выдачи результата; это ограничение не скрыто в отчёте.

Подробная итоговая сводка: [docs/final-verification.md](docs/final-verification.md). Методические допущения: [docs/assumptions.md](docs/assumptions.md).
