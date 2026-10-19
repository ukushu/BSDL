# Рефакторинг: план перевірок

Цей документ описує перевірки для змін у спільному video/audio/event/render коді, Surface/YUV і WAV модулях та Vulkan GPU backend. Автоматичний успіх `deus check .` підтверджує збірку й доступні тести в поточній конфігурації, але не замінює перевірку драйверів на їхніх цільових ОС.

## Спільні перевірки на кожній платформі

Запустити `deus check .` у конфігурації, що відповідає платформі. Команда має завершитись успішною збіркою та без помилок у тестах. Тести, які пропущені через відсутній драйвер або пристрій, записати як пропущені, а не як пройдені.

Переконатися, що набір `testautomation` перевіряє:

- Surface: створення й конвертацію форматів, clip rect, blend/color/alpha modulation, palette і color key, alternate images та властивості; конвертація не змінює стан джерела.
- YUV: порядок площин, planar/semiplanar/packed формати, розміри й крок рядка на парних та непарних розмірах.
- WAV: PCM, порядок `fmt`/`data` та невідомих chunk-ів, float і підтримувані companded/ADPCM декодери; некоректний RIFF має повертати помилку без витоку ресурсів.
- Video/render: режими дисплеїв, параметри й safe area вікна, fullscreen, window surface/texture, logical presentation, DPI coordinate round trips і mouse/touch/pen/drop перетворення, OpenGL/Vulkan loader lifecycle, fallback renderer і збереження помилки при невдалому створенні.
- Events/joystick: черга й callback-и подій, передачу тимчасової пам’яті між потоком і queued event, створення віртуального joystick/gamepad, читання та зміна кнопок/осей; перевірити підписи face buttons для ABXY, AXBY, BAYX і Sony стилів.
- Temporary event memory: створити event із `SDL_CreateTemporaryString`, додати в queue, peek/read його кілька разів, перевірити чинність рядка до видалення та повторне використання після; перевірити `SDL_FlushEvent`, drop/text/clipboard payload-и й SysWM stack-copy lifetime.
- Software geometry fallback: прямокутні quad-и, текстуровані та нетекстуровані кольорові quad-и, індекси 8/16/32-bit, raw geometry і fallback-трикутники; перевірити, що черга команд та відновлення draw state не змінилися.
- Streaming texture lock/update: native, YUV і palette-backed textures; lock-to-surface, dirty updates, unlock flush і readback після циклу lock/write/unlock.
- Window placement/display selection: point/rect display lookup, popup coordinate conversion, pending/fullscreen display selection and fallback to the primary display.
- Video sync hint: toggle `SDL_HINT_VIDEO_SYNC_WINDOW_OPERATIONS` at runtime and verify window operations follow the updated hint; confirm fullscreen-mode changes remain synchronous on macOS Cocoa transitions and do not affect behavior when the hint is disabled.
- Gamepad peripherals: on a controller that provides them, query touchpad count/fingers and finger state, sensor support/enabled state/data rate/data, and capacitive button state; verify invalid touchpad/finger indices return errors and optional output pointers are handled independently.
- Main-thread callbacks: from a worker thread queue callbacks with and without waiting, verify execution on the main thread and FIFO order; verify direct execution when called on the main thread and safe cancellation when the event subsystem quits with queued callbacks.
- Application lifecycle: send will-enter-background / did-enter-foreground around a live window and verify minimize, focus loss, focus restore and restored events; verify terminating and low-memory notifications reach event watchers.

## Матриця платформ

| Платформа та конфігурація | Що перевірити | Як підтвердити |
| --- | --- | --- |
| Linux X11, реальний X server | `SDL_video.c`, display/mode/window шляхи, fullscreen, OpenGL контекст, Vulkan surface, GPU Vulkan lifecycle і transfer/copy. Запустити віконні `video`/`render` тести та `testvulkan` за наявності сумісного GPU. | `deus check .`; перевірити, що тести, яким потрібен X11/GPU, реально виконались, а не були пропущені. Зафіксувати X server, GPU та драйвер. |
| Linux Wayland, реальний compositor | Збірка з `SDL_WAYLAND=ON`; створення/закриття вікон, зміни розміру, фокус і події вводу, clipboard, fullscreen/режими, OpenGL ES або Vulkan surface, GPU submit/present. Перевірити хоча б один compositor (наприклад, GNOME або KDE); другий compositor бажаний для протокольних відмінностей. | `deus check .` у Wayland сесії з backend явно вибраним через `SDL_VIDEODRIVER=wayland`; упевнитися, що не використано XWayland. Окремо зібрати й пройти тести з Wayland увімкненим. |
| Windows 10/11 x64, native session | Перевірити clean configure/build MSVC через Deus; desktop window create/destroy, resize, minimize/restore, focus/input, fullscreen enter/leave, display mode і high-DPI; OpenGL context та Vulkan surface lifecycle, якщо backend увімкнено. На реальному D3D12 пристрої перевірити GPU device create/destroy, command buffer submit, buffer upload/download, fence wait, render/readback і swapchain present. Якщо доступні обидва адаптери, повторити на інтегрованому й дискретному GPU. Перевірити вихід із помилкою при відсутньому/непідтримуваному backend без втрати SDL error. | Запустити `deus check .` у Windows native session з MSVC і зберегти повний лог. У CTest/automation зафіксувати Passed/Failed/Skipped; переконатися, що video/window тести виконалися саме на Windows backend, а GPU тести — на D3D12, а не завершилися пропуском. Якщо Vulkan увімкнений, виконати окремий прогін на Vulkan. Записати версію Windows, MSVC, Windows SDK, CMake options, GPU та driver. |
| macOS, native session на підтримуваній SDL архітектурі | Перевірити clean configure/build Xcode через Deus; window create/destroy, resize, minimize/restore, focus/input, fullscreen і display change; window surface/texture, Retina/high-DPI drawable та OpenGL wrapper лише на конфігураціях, де він доступний. На реальному Metal пристрої перевірити GPU device create/destroy, command buffer submit, upload/download, fence completion, render/readback, drawable acquisition і present. Перевірити помилкове завершення при недоступному Metal device без втрати SDL error. | Запустити `deus check .` у macOS native session і зберегти повний лог. У CTest/automation зафіксувати Passed/Failed/Skipped; переконатися, що video/window тести виконалися на Cocoa, а Metal тести справді виконалися на GPU. Зафіксувати ОС, архітектуру, Xcode/compiler, SDK/deployment target, CMake options, GPU та driver. Перевірити drawable у Retina режимі при двох scale factors, якщо доступні. |
| Linux headless/software (додаткова конфігурація) | Повторити форматні Surface/YUV/WAV, events, virtual joystick і software renderer без залежності від фізичного дисплея/GPU. Перевірити dummy video/audio драйвери. | `deus check .` з dummy драйверами. Ця конфігурація доповнює, але не замінює X11/Wayland/GPU перевірки. |

## Умови приймання

Для кожного рядка матриці зберегти ОС/версію, компілятор, CMake опції, відео backend, GPU та драйвер, підсумок `deus check .`, а також список пропущених тестів із причиною. Платформений пункт закритий лише коли відповідна конфігурація зібрана і її релевантні runtime тести виконались успішно.

### Послідовність native перевірки Windows і macOS

1. Запускати роботу з чистої конфігурації Deus на цільовій ОС; не переносити `.deus-build` між платформами. Зберегти вихідні параметри configure/build у логу.
2. Виконати повний `deus check .` один раз після завершення блоку змін. Build success окремо відзначити від тестового результату; невиконані через пристрій або backend тести вказати як skipped із причиною.
3. Переконатися, що віконні тести використовували native backend (`windows` або `cocoa`), а не dummy/offscreen. GPU тести мають показати фактичний backend та adapter/device; успішна компіляція GPU коду сама по собі runtime перевіркою не є.
4. Якщо один тест падає, зберегти точне ім’я/seed/log і звірити з наявними bug cards. Не закривати platform row за рахунок виключення відомого падіння чи перетворення його на skip.
5. У звіті вказати Passed, Failed і Skipped окремо та додати причину кожного skip. Вважати backend перевіреним лише за наявності фактичних runtime assertions для його device/window/present шляху.

### Конкретні перевірки Windows і macOS

Для кожної ОС виконати спільні автоматичні suites (`testautomation`, зокрема video, render, surface, events і joystick), а також окремо зафіксувати такі native сценарії:

| ОС | Native сценарії | Умови успіху |
| --- | --- | --- |
| Windows 10/11 | Запустити SDL app через Win32 backend; створити/закрити вікно, змінити розмір, згорнути/відновити, змінити фокус і перевірити клавіатуру/мишу; увійти/вийти з fullscreen; перевірити display enumeration та DPI; створити D3D12 renderer, очистити/намалювати, прочитати результат, показати кадр і повторно створити вікно. За наявності Vulkan окремо створити surface, показати кадр і знищити surface/device. | У журналі видно `windows` backend і фактичний D3D12/Vulkan adapter. Немає зависання або втрати SDL error при відмові у створенні backend/device. Перевірити resize/fullscreen циклом щонайменше кілька разів, щоб упіймати витоки та stale resources. |
| macOS | Запустити SDL app через Cocoa; створити/закрити вікно, змінити розмір, згорнути/відновити, змінити фокус та ввід; увійти/вийти з fullscreen і переключити дисплей; перевірити window surface/texture та Retina drawable; створити Metal renderer, очистити/намалювати, прочитати результат, отримати drawable, показати кадр і повторно створити вікно. OpenGL перевірити лише якщо він доступний у цільовій версії ОС; Vulkan — лише якщо зібраний відповідний loader/backend. | У журналі видно `cocoa` і Metal device. Перевірити drawable на дисплеях зі scale factor 1x та 2x, якщо доступні: розмір drawable відповідає pixel size, координати вводу лишаються логічними. Немає втрати SDL error при недоступному Metal/device. Повторити resize/fullscreen цикл для перевірки життєвого циклу drawable/resources. |

Не зараховувати збірку чи запуск без вікон як native runtime перевірку. Якщо фізичний GPU, другий scale factor або Vulkan недоступний, позначити конкретний пункт Skipped із причиною й залишити його відкритим.

### Перевірки поточного video refactor

Останні зміни перенесли display та HDR synchronization у `SDL_video_displays.c`, fullscreen/mode operations у `SDL_video_modes.c`, а визначення display і координат початкового вікна — у `SDL_video_windowplacement.c`. `SDL_video.c` лишається власником створення/знищення вікна та викликів backend. На кожній ОС виконати:

- Clean configure/build через `deus check .` після змін; на Windows перевірити MSVC project, на macOS — Xcode project. Не переносити build directory між ОС.
- Створити вікна з undefined/centered позицією на основному й додатковому дисплеї; перевірити більші за usable bounds розміри, явну позицію та display ID у window position flags.
- Переміщати вікно між дисплеями, змінювати display resolution/scale та HDR state; перевірити display-changed, display-scale-changed і HDR property events.
- Повторити fullscreen enter/leave, exclusive mode selection, перемикання дисплея та відновлення після resize/minimize. Перевірити Cocoa fullscreen space на macOS і native fullscreen/display mode на Windows.
- Для кожного сценарію звірити display ID, window bounds, DPI/pixel size та події з нативними даними ОС. Окремо записати backend, GPU, Passed/Failed/Skipped і причину пропуску.

Цей Linux прогін перевіряє спільний код і X11 поведінку; він не закриває Windows/macOS рядки матриці. Windows native/MSVC та macOS native/Xcode виконання лишаються обов'язковими перед платформним прийманням.

### Перевірки renderer refactor на Windows і macOS

Після переносу logical presentation, coordinate conversion та GPU custom render-state API виконати окремо на native Windows і macOS:

- Перевірити logical size, letterbox/overscan/integer scale, viewport і safe area; зіставити координати вікна, renderer output, mouse/touch/pen/drop події та pixel size після resize, зміни DPI і переміщення між дисплеями. На Windows використати різні DPI monitors; на macOS перевірити Retina 1x/2x drawable.
- Перевірити `SDL_SetRenderDrawColor`, draw color/alpha scale, blend mode, color scale mode та HDR output properties на кожному доступному backend. Порівняти framebuffer/readback для clear і primitives, перевірити послідовні зміни стану між кадрами, reset після resize та коректні SDL errors для непідтримуваного HDR/color mode. На Windows перевірити D3D12 і Vulkan, якщо доступні; на macOS — Metal.
- Перевірити renderer/window association, name/properties і output size для window renderer після resize, DPI change та window recreation; звірити pixel size на mixed-DPI Windows displays і Retina drawable на macOS. Перевірити offscreen та software renderer: output size має бути коректним згідно API.
- Перевірити geometry triangles з/без texture, 8/16/32-bit indices, автоматичний clamp/wrap UV та software fallback; повторити з logical scaling і після resize/DPI зміни, звіривши framebuffer/readback на кожній платформі.
- Перевірити streaming texture lock, lock-to-surface, update та unlock/readback на software і hardware renderer; для YUV і palette-backed texture звірити формат/площини та результат після flush на Windows і macOS.
- Зібрати `testgpurender_effects` і `testgpurender_msdf`; створити GPU render state, встановити vertex/fragment shader, sampler та uniform-и, намалювати кадр і перевірити readback. Повторити після resize/window recreation, щоб перевірити чергу команд і життєвий цикл ресурсів.
- Перевірити backend помилку/відсутній GPU та збереження SDL error. Зафіксувати реальний backend/device; для Windows перевірити D3D12 і Vulkan, якщо доступні, для macOS — Metal (Vulkan лише якщо backend зібраний).

`deus check .` не запускає `testgpurender_effects` і `testgpurender_msdf` окремими CTest цілями. Їхній build і native runtime результат записувати окремо; пропуск через відсутній backend/GPU позначати Skipped із причиною.

### Перевірки event callback refactor на Windows і macOS

Після переносу черги callback-ів головного потоку виконати `events_testRunOnMainThread` на Windows native/MSVC і macOS native/Xcode. Перевірити постановку з worker thread у режимах wait і no-wait, виконання callback саме на main thread, завершення waiting thread, прямий виклик із main thread та cancellation queued callback під час завершення event subsystem. Зафіксувати Passed/Failed/Skipped; не зараховувати dummy/offscreen запуск як перевірку native thread wake-up.

Поточне середовище Linux має активну X11 сесію й AMD Radeon RX 5700 (RADV). Конфігурація Deus з `SDL_X11=ON`, `SDL_WAYLAND=ON`, `SDLTEST_VIDEO_DRIVER=x11` успішно збирається. У повному прогоні `deus check .` від 2026-10-19 усі 25 CTest цілей виконались: 23 пройшли, дві (`testautomation` і `testautomation-no-simd`) завершились помилкою через відомий `render_testColorspaceLinear` (картка 000007). У звичайному automation target було 4 пропуски, у no-SIMD — 12; пропуск не є успіхом і має оцінюватися за потрібним драйвером. `video_testFullscreenEnterLeave` і `mouse_getMouseFocus` у цьому прогоні пройшли; їхні попередні нестабільні результати лишаються зафіксованими окремо в картках 000008 і 000040.

На реальному RX 5700 пройшли Vulkan device lifecycle, buffer copy, повторне створення малих буферів і renderer clear/lines/points/geometry/readback, 1024 зміни color scale та `SDL_RenderPresent`. Тест Vulkan command queue перевіряє примітиви й довгу серію змін кольору окремо. Змішаний сценарій, що виявив no-SIMD зміну кольорів попередніх ліній, не включений до успішних перевірок; його окрема помилка зареєстрована в картці 000043, і її треба виправити там та додати регресійне покриття. Помилка acquire наступного swapchain image також лишається неперевіреним failure path (картка 000044). Wayland лише компілюється: активного compositor немає. Wayland runtime, Windows D3D12 і macOS Metal ще потребують відповідних native-перевірок, включно з GPU presenting.

Software geometry fallback винесено до `src/render/software/SDL_render_sw_geometry.c` з callback-ом для локальної черги `SDL_render.c`; build-реєстрації та `Dox/ProjTree.md` оновлено. Після переносу `deus check .` зібрав конфігурацію успішно; підсумок тестів лишився 23/25 через відомий 000007, без нової помилки, що стосується geometry.

Групу вибору display та перетворення координат вікон винесено до `src/video/SDL_video_windowplacement.c`; `SDL_video.c` залишився власником window lifecycle. Build-реєстрації й внутрішній прототип `SDL_GetDisplayAtOrigin` оновлено. `deus check .` зібрав код; runtime підсумок лишився 23/25 через 000007.

Мапування стилю gamepad і фізичного типу контролера на підписи кнопок винесено до `src/joystick/SDL_gamepad_labels.c`; `SDL_gamepad.c` лишився відповідальним за стан відкритого gamepad. Додано перевірку ABXY, GameCube, Nintendo Switch Pro та PlayStation labels у `test/testautomation_joystick.c`; вона пройшла у звичайному й no-SIMD automation прогонах. CMake знаходить файл автоматично, Visual Studio і Xcode registrations оновлено.

Парсинг GUID/name/button mapping, CRC normalization і формування mapping-рядка винесено до `src/joystick/SDL_gamepad_mapping.c`; `SDL_gamepad.c` лишився власником списку й життєвого циклу mappings. Наявні `joystick_testMappings`, `joystick_mappingElementParsing` та gamepad label checks пройшли у звичайному й no-SIMD automation. У цій області вже зареєстрований окремий OOM-дефект (картка 000033); перенесення його не виправляє.

`SDL_UpdateFullscreenMode` розділено на локальні операції входу та виходу з fullscreen; керування платформеним станом і API лишилося у `SDL_video.c`. Існуючий `video_testFullscreenEnterLeave` пройшов у поточному X11 прогоні.

Вибір video backend винесено з `SDL_InitVideo` у локальну `SDL_CreateVideoDevice`; ініціалізація підсистем і cleanup лишилися в `SDL_InitVideo`. Linux `deus check .` зібрав код; відео тести пройшли, крім попередньо відомих падінь renderer у двох automation цілях.

Перемикання GL/Vulkan бібліотек і знищення старого native window винесено в `SDL_RecreateWindowGraphics`; послідовність unload → backend destroy → load збережена. У наступному Linux `deus check .` `video_recreateWindowSettings` пройшов (12/12 assertions); повний результат — 23/25 CTest через той самий відомий `render_testColorspaceLinear` (000007).

Вибір активного video backend або bootstrap fallback для message box винесено з `SDL_ShowMessageBox` у `SDL_ShowMessageBoxWithFallback`; загальна обробка error, UI state і message-box counter лишилась у виклику. Поточні automation тести не викликають message-box backend; перевірено компіляцію, а повний прогін зберіг попередній результат 23/25 із відомим 000007.

`deus check .` від 2026-10-19 після локальних переносів завершив configure/build успішно: CTest 23/25. Дві цілі (`testautomation` і `testautomation-no-simd`) падають лише на відомому `render_testColorspaceLinear` (картка 000007). У звичайному automation було 433 assertions: 428 passed, 1 failed, 4 skipped; у no-SIMD — 433: 420 passed, 1 failed, 12 skipped. Gamepad label/mapping checks і `video_testFullscreenEnterLeave` пройшли; інші 23 CTest цілі пройшли.

Geometry fallback у `SDL_RenderTextureRotated` винесено в локальну `SDL_QueueRotatedTextureGeometry`; параметри та формули UV/повороту збережено, шлях QueueCopyEx не змінено. Існуючий `render_testRGBSurfaceNoAlpha` викликає обернений рендер і перевіряє результат на пікселях, але software renderer використовує власний QueueCopyEx, тому цей тест не підтверджує окремо geometry fallback. Після зміни `deus check .` пройшов configure/build; CTest — 23/25, лише дві відомі помилки `render_testColorspaceLinear` (000007). У кожному automation було 433 assertions; звичайний прогін: 428 passed, 1 failed, 4 skipped; no-SIMD: 420 passed, 1 failed, 12 skipped. Windows/macOS native runtime перевірки вище лишаються невиконаними в цьому Linux середовищі.

Налаштування output colorspace та sRGB→linear/HDR10 перетворення винесено в `src/render/SDL_render_color.c`; backend-и лишилися на спільних внутрішніх функціях. Файл додано до Visual Studio, Visual Studio GDK та Xcode проектів; CMake підбирає його через наявний glob джерел.

Перетворення renderer/window coordinates і відповідних mouse/touch/pen/drop подій винесено в `src/render/SDL_render_coordinates.c`. Формули logical presentation, DPI scale та viewport збережено; event conversion лишається поруч із цими перетвореннями. Файл зареєстровано в CMake auto-discovery, Visual Studio, Visual Studio GDK та Xcode.

Renderer defaults для texture scale/address mode перенесено до `SDL_render_texture_state.c` поруч із texture state API. Перевірки renderer/texture handle зведено до спільного приватного `SDL_render_internal.h`; значення defaults і перевірку параметрів не змінено.

`deus check .` після переносу coordinate-функцій, texture defaults і спільних validation macros пройшов configure/build; CTest — 23/25. Обидві цілі падають на відомому `render_testColorspaceLinear` (000007). У звичайному automation: 433 assertions, 428 passed, 1 failed, 4 skipped; у no-SIMD: 433, 419 passed, 2 failed, 12 skipped — крім colorspace, нестабільний `mouse_getMouseFocus` (000040) теж упав у цьому прогоні. Один попередній прогін завершився segfault під час `render_testVulkanCommandQueue` після `Queue repeated Vulkan color changes`; два наступні прогони його не відтворили, причина невідома. Це не приписується переносу coordinate-функцій без відтворення/стеку. Windows/macOS native runtime перевірки лишаються невиконаними.

Window/display mapping, display scale та HDR property synchronization разом із display-change callbacks перенесено до наявного `SDL_video_displays.c`. Використання активного `SDL_VideoDevice` і валідація window handle збережені; callback-функції та внутрішній display lookup оголошені у `SDL_sysvideo.h` для викликів із window lifecycle. Після переносу `deus check .` пройшов configure/build, CTest — 23/25; обидва automation failures включають відоме 000007, no-SIMD додатково має нестабільний `mouse_getMouseFocus` (000040). Інші 23 CTest цілі пройшли.

Fullscreen enter/leave, display mode switching та `SDL_UpdateFullscreenMode` перенесено до наявного `SDL_video_modes.c`, де вже зібрані display mode operations. Core лишив `SDL_SyncIfRequired`; fullscreen код читає sync hint через приватний accessor, а window validation зберігає ті самі помилки. `deus check .` пройшов configure/build; CTest — 23/25. В обох automation цілях лишилося лише відоме `render_testColorspaceLinear` (000007): звичайний прогін 428/433 passed, 1 failed, 4 skipped; no-SIMD 420/433 passed, 1 failed, 12 skipped. Решта 23 CTest цілі пройшли.

Початкове визначення centered/undefined window position винесено до `SDL_video_windowplacement.c`; формули, прапорці undefined координат і вибір display збережені. Наявні `video_getSetWindowPosition` та `video_testFullscreenEnterLeave` пройшли в обох automation конфігураціях. Останній `deus check .` (2026-10-19) зібрався успішно, CTest — 23/25: обидва automation прогони впали лише на відомому `render_testColorspaceLinear` (000007; по 1 failed test case у кожному); звичайний: 428/433 passed, 4 skipped; no-SIMD: 420/433 passed, 12 skipped. Інші 23 CTest цілі пройшли. `git diff --check` чистий. Windows/MSVC і macOS/Xcode runtime перевірки ще не виконані.

`SDL_GetWindowPosition` і `SDL_GetWindowSizeInPixels` перенесено до того ж placement-модуля поруч із дисплейними координатами та pixel-density fallback. Логіка Cocoa fullscreen position і backend override збережені. Існуючі `video_getSetWindowPosition`, `video_createMinimized` та `video_createMaximized` покривають ці API; повний прогін після саме цього переносу ще не виконувався.

Розмір вікна, aspect ratio, мінімальні й максимальні межі разом із їхніми getters винесено в `SDL_video_windowgeometry.c`. Умови валідації, clamp-порядок, backend callbacks і conditional sync збережені; `SDL_SyncIfRequired` тепер доступна через внутрішній відео header. Існуючі `video_getSetWindowSize`, `video_getSetWindowMinimumSize` та `video_getSetWindowMaximumSize` покривають API. Новий файл додано до Visual Studio, GDK та Xcode build projects.

Після переносу geometry API `deus check .` успішно зібрав проєкт; CTest — 23/25. `video_getSetWindowPosition`, size/minimum/maximum tests та create-minimized/maximized пройшли в обох automation прогонах. Єдині два падіння — відомий `render_testColorspaceLinear` (000007): звичайний прогін 428/433 passed, 1 failed, 4 skipped; no-SIMD 420/433 passed, 1 failed, 12 skipped. `git diff --check` чистий.

Window list/ID/parent/properties/flags accessors винесено до `SDL_video_windowregistry.c`; пошук ID продовжує обходити той самий window list, а invalid ID зберігає SDL error. Файл підключено у Visual Studio, GDK і Xcode. Після цього переносу `deus check .` зібрався; CTest — 23/25 із тими самими двома `render_testColorspaceLinear` помилками (000007). `video_getWindowFlags`, `video_getWindowId` і `video_destroyWindowUnlinksList` пройшли в обох automation прогонах; звичайний automation — 428/433 passed, 4 skipped; no-SIMD — 420/433 passed, 12 skipped. Visual Studio XML, Xcode project lint і `git diff --check` пройшли.

GPU custom render-state create/update/destroy API винесено до `SDL_render_gpu_state.c`; dependency-aware command flush лишився в renderer core як внутрішній helper. Виправлено OOM failure path додавання uniform buffer: після realloc state більше не лишається з dangling array pointer, якщо allocation uniform data падає. `testgpurender_effects` і `testgpurender_msdf` використовують цей API, але `deus check .` не запускає їх окремими CTest цілями; runtime GPU перевірка лишається платформною. Файл додано до Visual Studio, GDK і Xcode projects. Після переносу `deus check .` зібрався; CTest — 23/25, дві automation цілі впали лише на відомому `render_testColorspaceLinear` (000007): звичайна 428/433 passed, 1 failed, 4 skipped; no-SIMD 420/433 passed, 1 failed, 12 skipped. `git diff --check`, Visual Studio XML і Xcode project lint пройшли.

Public logical presentation setter/getters і safe-area conversion винесено в `SDL_render_coordinates.c` поруч із coordinate conversion. Перерахунок viewport/command state лишився у core як `SDL_UpdateLogicalPresentation`, доступний тільки через внутрішній renderer header. `render_testLogicalSize` і `render_testLogicalLinesGeometry` покривають logical presentation; `testwm` і `testsprite` викликають safe-area API.

Після цих переносів `deus check .` успішно зібрався; `render_testLogicalSize` і `render_testLogicalLinesGeometry` пройшли в обох automation targets. CTest — 23/25, єдині дві помилки — відомий `render_testColorspaceLinear` (000007): звичайний automation 428/433 passed, 1 failed, 4 skipped; no-SIMD 420/433 passed, 1 failed, 12 skipped. `git diff --check` чистий.

Gamepad blacklist і VID/PID allow/ignore lists перенесено в `SDL_gamepad_filter.c`; init/shutdown списків зберігаються під joystick lock. Остання збірка `SDL3-shared` пройшла без попереджень; Visual Studio project XML, Xcode project lint і `git diff --check` пройшли. Після цього переносу gamepad runtime тести ще не запускалися.

Texture format selection, fallback creation, texture initialization, create/from-surface APIs і texture size/property access винесено в `SDL_render_texture_create.c`. Розрахунок pixel viewport/clip лишився спільним у renderer core і оголошений через внутрішній renderer header. `SDL3-shared` зібрався; повний automation прогін після переносу не запускався.

Texture palette attachment/detachment та palette/property/size access винесено в наявний `SDL_render_texture_state.c`. Dependency-aware palette flush лишився у core як внутрішній helper. `SDL3-shared` після переносу зібрався; runtime automation не запускався.

`deus check .` після gamepad filter і renderer texture refactor (2026-10-19) пройшов configure/build, CTest — 23/25. Звичайний automation: 428/433 passed, `render_testColorspaceLinear` failed (000007), 4 skipped; no-SIMD: 419/433 passed, `mouse_getMouseFocus` failed (000040) і `render_testColorspaceLinear` failed (000007), 12 skipped. Решта CTest цілей пройшла. Збережені seeds: normal `R4F2MTWKAXTQKHKA`, no-SIMD `QC4OATU3NGMXGRP7`.

Point/line/rect/fill-rect APIs та їх queue/geometry fallback винесено в `SDL_render_primitives.c`. Після переносу пройшли вибіркові тести `render_testLogicalLinesGeometry`, `render_testSoftwareGeometryQuads`, `render_testTextureCreationProperties` і `render_testSoftwareTexturePalette` (кожен окремо, 1/1). `SDL3-shared` зібрався; повний `deus check .` після цього переносу не запускався.

Texture draw APIs (copy, affine, rotated, tiled, 9-grid) винесено в `SDL_render_texture_draw.c`; palette synchronization використовується спільно з geometry API через `SDL_render_texture_state.c`. `SDL3-shared` зібрався, а `render_testGeometryAutoTextureAddress`, `render_testTextureCreationProperties`, `render_testTextureState` і `render_testSoftwareTextureFallback` пройшли окремо (1/1 кожен). Повний `deus check .` після цього переносу не запускався.

Viewport, clip rect і render scale APIs перенесено в `SDL_render_coordinates.c`; спільні pixel viewport/clip та queue invalidation викликаються через internal renderer helpers. `render_testLogicalSize`, `render_testLogicalLinesGeometry` і `render_testSoftwareRendererFlushSequence` пройшли вибірково (1/1 кожен). `SDL3-shared` зібрався; повний прогін після переносу не запускався.

Draw color, color scale, blend mode, color scale mode і HDR output state APIs винесено в `SDL_render_draw_state.c`; renderer lifecycle та command queue лишилися в core. `SDL3-shared` зібрався, `render_testBlendModes` пройшов (1/1), `git diff --check` чистий. Файл зареєстровано у Visual Studio, GDK і Xcode projects. Повний automation прогін після переносу не запускався.

Renderer/window association, renderer name/properties і output-size accessors винесено в `SDL_render_info.c`; вибір драйвера, створення та lifecycle лишилися в core. `SDL3-shared` зібрався, `render_testSoftwareRendererFlushSequence` пройшов (1/1). Native Windows-перевірка включає mixed-DPI resize, macOS — Retina drawable/window recreation; обидві мають покривати offscreen/software renderer. Повний прогін після переносу не запускався.

`SDL_RenderGeometry`/`SDL_RenderGeometryRaw` та перевірки індексів/UV винесено в `SDL_render_geometry.c`; command queue geometry API лишився у renderer core. Після `SDL3-shared` build пройшов `render_testGeometryRawIndexWidths` (1/1); Visual Studio XML і Xcode project перевірено. Перевірити software fallback, texture palette sync, auto UV clamp/wrap та 8/16/32-bit індекси на Windows mixed-DPI і macOS Retina. Повний прогін після переносу не запускався.

Стан `SDL_HINT_VIDEO_SYNC_WINDOW_OPERATIONS`, його callback і `SDL_SyncIfRequired` винесено в `SDL_video_sync.c`; callback як і раніше реєструється після успішної video initialization. Після цього `SDL3-shared` зібрався без попереджень, а `video_testFullscreenEnterLeave` пройшов (1/1) на X11. Перевірка hint runtime toggle та синхронного fullscreen mode transition на Cocoa лишається платформною. Xcode project registrations перевірені: всі 1210 object IDs мають 24 символи, унікальні, а посилання резолвяться.

Touchpad, sensor і capsense Gamepad API винесено в `SDL_gamepad_peripherals.c`; залежність від private `SDL_Gamepad` struct не додавалася. `joystick_testVirtual` тепер перевіряє touchpad fingers, invalid touchpad error, sensor presence/enabled/rate/data на virtual gamepad; build `testautomation` і фокусний тест пройшли (1/1). Native Windows/macOS перевірка лишається для фізичних touchpad/sensor/capsense пристроїв, якщо вони доступні.

Streaming texture lock/unlock API, включно з YUV, palette surface і native texture paths та `SDL_LockTextureToSurface`, винесено з update module до `SDL_render_texture_lock.c`. Спільні update/YUV upload функції й lock/unlock semantics залишені без змін. `SDL3-shared` зібрався, а `render_testSoftwareTextureLock` пройшов (1/1) з lock-to-surface, unlock, render і pixel readback. Перевірки update, palette/YUV і readback для Windows/macOS додані до матриці; повний прогін після переносу не запускався.

Fullscreen display/window coordination винесено з `SDL_video_modes.c` у `SDL_video_fullscreen.c`; display mode inventory і mode selection лишилися окремо. `SDL3-shared` зібрався, `video_testFullscreenEnterLeave` пройшов (1/1) на X11. Native platform checklist покриває enter/leave, exclusive mode restore, display changes і Cocoa fullscreen Space transitions; ці Windows/macOS runtime перевірки лишаються відкритими.

Display/window association, display-change handling, HDR propagation та window pixel/content scale винесено до `SDL_video_windowdisplay.c`; display list/registry/geometry accessors лишилися в `SDL_video_displays.c`. Після `SDL3-shared` build пройшов увесь Video automation suite на X11: 35 passed, 0 failed, 1 skipped; до нього входили display registry і fullscreen transition перевірки. Windows mixed-DPI і macOS Retina сценарії залишаються в platform matrix.

Чергу callback-ів головного потоку винесено з `SDL_events.c` у `SDL_events_mainthread.c`; `SDL_SendWakeupEvent` лишається у event core і викликається через внутрішній прототип. `SDL3-shared` та `testautomation` зібралися; `events_mainThreadCallbacks` пройшов 9/9 assertions на X11. Windows native wake-up і macOS native thread checks лишаються відкритими за platform checklist.

Logical presentation, перерахунок pixel viewport/clip і DPI для main view перенесено з `SDL_render.c` у `SDL_render_coordinates.c`; вибір destination rect відділено від оновлення renderer state. `SDL3-shared` і `testautomation` зібралися; `render_testLogicalSize` пройшов на X11 після цього поділу. Windows mixed-DPI та macOS Retina перевірки лишаються відкритими за renderer platform checklist.

Application lifecycle callbacks винесено з `SDL_video.c` до `SDL_video_application.c`; функції зберігають події завершення/пам’яті та цикли minimize/focus/restore для вікон. `SDL3-shared` і `testautomation` зібралися. Lifecycle runtime перевірки на iOS/Android лишаються відкритими за спільним checklist.

Safe-area inset handling, change notification і query перенесено з `SDL_video.c` до `SDL_video_windowgeometry.c`; resize callback використовує спільний internal checker. `SDL3-shared` і `testautomation` зібралися; перевірки safe-area подій на пристроях із вирізом лишаються платформним пунктом.

Thread-local temporary memory ownership і керування TLS list винесено з `SDL_events.c` до `SDL_events_temporary_memory.c`; event core зберігає лише зв’язування payload-ів за типом події. `SDL3-shared` і `testautomation` зібралися, Events suite пройшов 5/5 на X11. Повного прямого покриття всіх payload lifetime сценаріїв у suite немає; вони внесені в checklist.

Розбір синтаксису input/output binding перенесено з `SDL_gamepad.c` до `SDL_gamepad_mapping.c`; core передає parser-у mapping string, а сам зберігає прикріплення binding до gamepad. `SDL3-shared` і `testautomation` зібралися; `joystick_mappingElementParsing` пройшов 18/18 assertions. Mapping-label тести лишаються окремою перевіркою.
