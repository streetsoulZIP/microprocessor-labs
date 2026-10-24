# Визуальная симуляция в VS Code

Расширение **Wokwi for VS Code 4.0.0** уже установлено. Откройте `microprocessor-labs.code-workspace` в VS Code. Каждый task lab00 и оба starter-проекта lab01/lab02 добавлены как отдельные корни workspace, поэтому рядом с `diagram.json` лежит свой `wokwi.toml`.

1. Соберите выбранный скетч из терминала, например `./scripts/build.ps1 -Lab lab00 -Sketch task08`.
2. Нажмите `F1` -> `Wokwi: Select Config File` и выберите `wokwi.toml` требуемого task.
3. Нажмите `F1` -> `Wokwi: Start Simulator`. Откроется интерактивная схема: LED видны на макете, pushbutton нажимается мышью, potentiometer вращается мышью.
4. При изменении исходника снова запустите build-скрипт и перезапустите симулятор: Wokwi VS Code не компилирует Arduino-скетч самостоятельно.

Все пути в `wokwi.toml` относительны к своему файлу и используют `/`; они ведут к Arduino CLI `build/<lab>/<sketch>/<sketch>.ino.hex` и `.elf`.

## Лицензия и бесплатная альтернатива

Для активации расширения выполните `F1` -> `Wokwi: Request a new License`, подтвердите открытие браузера и войдите/создайте бесплатный аккаунт Wokwi. Это авторизационное действие не может быть выполнено агентом. Если не хотите активировать расширение, используйте браузерный Wokwi: откройте https://wokwi.com/projects/new/arduino-uno и перенесите содержимое `diagram.json` и уже собранный firmware по инструкции Wokwi, либо оставьте автоматическую проверку через `scripts/simulate.ps1`.
