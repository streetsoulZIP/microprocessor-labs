# Lab02 - автомобильный светофор двумя методами

| Вариант | Исходник | Wokwi |
|---|---|---|
| switch-case | `switch_fsm/switch_fsm.ino` | `switch_fsm/diagram.json`, `switch_fsm/wokwi.toml` |
| table-driven | `table_fsm/table_fsm.ino` | `table_fsm/diagram.json`, `table_fsm/wokwi.toml` |

Подключение одинаково: D13 red, D12 yellow, D11 green; D2 pedestrian, D3 emergency, D4 night. Все кнопки замыкают на GND и используют `INPUT_PULLUP`.

Для визуального запуска дважды щёлкнуть `open-switch-wokwi.cmd` или `open-table-wokwi.cmd`. CMD собирает прошивку и открывает соответствующую папку в VS Code. Затем выполнить `F1 -> Wokwi: Start Simulator`; Wokwi запускает уже собранные HEX/ELF. Нужна ранее активированная лицензия Wokwi VS Code. Автоматический запуск UI расширения из CMD не поддерживается надёжным API, поэтому это единственный ручной шаг.
