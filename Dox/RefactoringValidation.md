# Рефакторинг: план перевірок

Цей документ описує перевірки для змін у спільному video/audio/event/render коді, Surface/YUV і WAV модулях та Vulkan GPU backend. Автоматичний успіх `deus check .` підтверджує збірку й доступні тести в поточній конфігурації, але не замінює перевірку драйверів на їхніх цільових ОС.

## Спільні перевірки на кожній платформі

Запустити `deus check .` у конфігурації, що відповідає платформі. Команда має завершитись успішною збіркою та без помилок у тестах. Тести, які пропущені через відсутній драйвер або пристрій, записати як пропущені, а не як пройдені.

Переконатися, що набір `testautomation` перевіряє:

- Surface: створення й конвертацію форматів, clip rect, blend/color/alpha modulation, palette і color key, alternate images та властивості; конвертація не змінює стан джерела.
- YUV: порядок площин, planar/semiplanar/packed формати, розміри й крок рядка на парних та непарних розмірах.
- WAV: PCM, порядок `fmt`/`data` та невідомих chunk-ів, float і підтримувані companded/ADPCM декодери; некоректний RIFF має повертати помилку без витоку ресурсів.
- Video/render: режими дисплеїв, параметри вікна, fullscreen, window surface/texture, OpenGL/Vulkan loader lifecycle, fallback renderer і збереження помилки при невдалому створенні.
- Events/joystick: черга й callback-и подій, створення віртуального joystick/gamepad, читання та зміна кнопок/осей.

## Матриця платформ

| Платформа та конфігурація | Що перевірити | Як підтвердити |
| --- | --- | --- |
| Linux X11, реальний X server | `SDL_video.c`, display/mode/window шляхи, fullscreen, OpenGL контекст, Vulkan surface, GPU Vulkan lifecycle і transfer/copy. Запустити віконні `video`/`render` тести та `testvulkan` за наявності сумісного GPU. | `deus check .`; перевірити, що тести, яким потрібен X11/GPU, реально виконались, а не були пропущені. Зафіксувати X server, GPU та драйвер. |
| Linux Wayland, реальний compositor | Збірка з `SDL_WAYLAND=ON`; створення/закриття вікон, зміни розміру, фокус і події вводу, clipboard, fullscreen/режими, OpenGL ES або Vulkan surface, GPU submit/present. Перевірити хоча б один compositor (наприклад, GNOME або KDE); другий compositor бажаний для протокольних відмінностей. | `deus check .` у Wayland сесії з backend явно вибраним через `SDL_VIDEODRIVER=wayland`; упевнитися, що не використано XWayland. Окремо зібрати й пройти тести з Wayland увімкненим. |
| Windows 10/11 x64 | Збірка MSVC/CMake; display/window lifecycle, fullscreen, OpenGL/Vulkan surface; GPU D3D12 create/destroy, buffer upload/download, fences, render. Перевірити і дискретний/інтегрований адаптери, якщо доступні. | `deus check .`; запустити video/render/GPU перевірки на D3D12 з реальним пристроєм. Vulkan перевірити окремо, якщо він увімкнений у збірці. |
| macOS на підтримуваній SDL архітектурі | Збірка Xcode/CMake; display/window lifecycle, window surface/texture, OpenGL wrapper де підтримується; Metal GPU device, upload/download, fences і present. Перевірити Retina/high-DPI розмір drawable. | `deus check .`; GPU перевірки виконати на реальному Metal пристрої. Збірку й назву схеми записати в результаті. |
| Linux headless/software (додаткова конфігурація) | Повторити форматні Surface/YUV/WAV, events, virtual joystick і software renderer без залежності від фізичного дисплея/GPU. Перевірити dummy video/audio драйвери. | `deus check .` з dummy драйверами. Ця конфігурація доповнює, але не замінює X11/Wayland/GPU перевірки. |

## Умови приймання

Для кожного рядка матриці зберегти ОС/версію, компілятор, CMake опції, відео backend, GPU та драйвер, підсумок `deus check .`, а також список пропущених тестів із причиною. Платформений пункт закритий лише коли відповідна конфігурація зібрана і її релевантні runtime тести виконались успішно.

Поточне середовище Linux має активну X11 сесію й AMD Radeon RX 5700 (RADV). Конфігурація Deus з `SDL_X11=ON`, `SDL_WAYLAND=ON`, `SDLTEST_VIDEO_DRIVER=x11` успішно збирається. Vulkan device lifecycle, buffer copy, повторне створення малих буферів і Vulkan renderer clear/lines/points/geometry/readback, 1024 зміни color scale та успішний `SDL_RenderPresent` виконалися на реальному GPU та пройшли. Помилка acquire наступного swapchain image залишається окремим неперевіреним failure path (картка 000044). Спільна черга ранніх ліній і цих 1024 змін у no-SIMD конфігурації змінювала пікселі ліній до рефакторингу (картка 000043); поточний тест читає ці два сценарії окремо. Повний X11 прогін `deus check .` має зареєстровані збої `render_testColorspaceLinear` (картка 000007), нестабільний `video_testFullscreenEnterLeave` (картка 000008) і нестабільний `mouse_getMouseFocus` (картка 000040); не вважати його зеленим. Wayland лише компілюється: активного compositor немає. Wayland runtime, Windows D3D12 і macOS Metal ще потребують відповідних перевірок, включно з GPU presenting на цих платформах.
