# PROJECT TREE

```
./
├── android-project/
│   ├── app/
│   │   ├── jni/
│   │   │   ├── src/
│   │   │   │   ├── Android.mk
│   │   │   │   ├── CMakeLists.txt
│   │   │   │   └── YourSourceHere.c
│   │   │   ├── Android.mk
│   │   │   ├── Application.mk
│   │   │   └── CMakeLists.txt
│   │   ├── src/
│   │   │   └── main/
│   │   │       ├── java/
│   │   │       │   └── org/
│   │   │       │       └── libsdl/
│   │   │       │           └── app/
│   │   │       │               ├── HIDDevice.java
│   │   │       │               ├── HIDDeviceBLESteamController.java
│   │   │       │               ├── HIDDeviceManager.java
│   │   │       │               ├── HIDDeviceUSB.java
│   │   │       │               ├── SDL.java
│   │   │       │               ├── SDLActivity.java
│   │   │       │               ├── SDLAudioManager.java
│   │   │       │               ├── SDLControllerManager.java
│   │   │       │               ├── SDLDummyEdit.java
│   │   │       │               ├── SDLInputConnection.java
│   │   │       │               ├── SDLSensorManager.java
│   │   │       │               └── SDLSurface.java
│   │   │       ├── res/
│   │   │       │   ├── mipmap-hdpi/
│   │   │       │   │   └── ic_launcher.png
│   │   │       │   ├── mipmap-mdpi/
│   │   │       │   │   └── ic_launcher.png
│   │   │       │   ├── mipmap-xhdpi/
│   │   │       │   │   └── ic_launcher.png
│   │   │       │   ├── mipmap-xxhdpi/
│   │   │       │   │   └── ic_launcher.png
│   │   │       │   ├── mipmap-xxxhdpi/
│   │   │       │   │   └── ic_launcher.png
│   │   │       │   └── values/
│   │   │       │       ├── colors.xml
│   │   │       │       ├── strings.xml
│   │   │       │       └── styles.xml
│   │   │       └── AndroidManifest.xml
│   │   ├── build.gradle
│   │   └── proguard-rules.pro
│   ├── gradle/
│   │   └── wrapper/
│   │       ├── gradle-wrapper.jar
│   │       └── gradle-wrapper.properties
│   ├── build.gradle
│   ├── gradle.properties
│   ├── gradlew
│   ├── gradlew.bat
│   └── settings.gradle
├── build/
│   ├── CMakeFiles/
│   │   ├── 3.28.3/
│   │   │   ├── CompilerIdC/
│   │   │   │   ├── tmp/
│   │   │   │   ├── a.out
│   │   │   │   └── CMakeCCompilerId.c
│   │   │   ├── CompilerIdCXX/
│   │   │   │   ├── tmp/
│   │   │   │   ├── a.out
│   │   │   │   └── CMakeCXXCompilerId.cpp
│   │   │   ├── CMakeCCompiler.cmake
│   │   │   ├── CMakeCXXCompiler.cmake
│   │   │   ├── CMakeDetermineCompilerABI_C.bin
│   │   │   ├── CMakeDetermineCompilerABI_CXX.bin
│   │   │   └── CMakeSystem.cmake
│   │   ├── CMakeTmp/
│   │   │   ├── SDL_detect_arch/
│   │   │   │   ├── CMakeFiles/
│   │   │   │   │   └── CMakeTmp/
│   │   │   │   └── bin
│   │   │   └── SDL_detect_arch.c
│   │   ├── Export/
│   │   │   └── 35815d1d52a6ea1175d74784b559bdb6/
│   │   │       ├── SDL3headersTargets.cmake
│   │   │       ├── SDL3sharedTargets-relwithdebinfo.cmake
│   │   │       ├── SDL3sharedTargets.cmake
│   │   │       ├── SDL3testTargets-relwithdebinfo.cmake
│   │   │       └── SDL3testTargets.cmake
│   │   ├── git-data/
│   │   │   ├── grabRef.cmake
│   │   │   ├── HEAD
│   │   │   └── head-ref
│   │   ├── pkgRedirects/
│   │   ├── SDL_uclibc.dir/
│   │   │   ├── src/
│   │   │   │   └── libm/
│   │   │   │       ├── e_atan2.c.o
│   │   │   │       ├── e_atan2.c.o.d
│   │   │   │       ├── e_exp.c.o
│   │   │   │       ├── e_exp.c.o.d
│   │   │   │       ├── e_fmod.c.o
│   │   │   │       ├── e_fmod.c.o.d
│   │   │   │       ├── e_log.c.o
│   │   │   │       ├── e_log.c.o.d
│   │   │   │       ├── e_log10.c.o
│   │   │   │       ├── e_log10.c.o.d
│   │   │   │       ├── e_pow.c.o
│   │   │   │       ├── e_pow.c.o.d
│   │   │   │       ├── e_rem_pio2.c.o
│   │   │   │       ├── e_rem_pio2.c.o.d
│   │   │   │       ├── e_sqrt.c.o
│   │   │   │       ├── e_sqrt.c.o.d
│   │   │   │       ├── k_cos.c.o
│   │   │   │       ├── k_cos.c.o.d
│   │   │   │       ├── k_rem_pio2.c.o
│   │   │   │       ├── k_rem_pio2.c.o.d
│   │   │   │       ├── k_sin.c.o
│   │   │   │       ├── k_sin.c.o.d
│   │   │   │       ├── k_tan.c.o
│   │   │   │       ├── k_tan.c.o.d
│   │   │   │       ├── s_atan.c.o
│   │   │   │       ├── s_atan.c.o.d
│   │   │   │       ├── s_copysign.c.o
│   │   │   │       ├── s_copysign.c.o.d
│   │   │   │       ├── s_cos.c.o
│   │   │   │       ├── s_cos.c.o.d
│   │   │   │       ├── s_fabs.c.o
│   │   │   │       ├── s_fabs.c.o.d
│   │   │   │       ├── s_floor.c.o
│   │   │   │       ├── s_floor.c.o.d
│   │   │   │       ├── s_isinf.c.o
│   │   │   │       ├── s_isinf.c.o.d
│   │   │   │       ├── s_isinff.c.o
│   │   │   │       ├── s_isinff.c.o.d
│   │   │   │       ├── s_isnan.c.o
│   │   │   │       ├── s_isnan.c.o.d
│   │   │   │       ├── s_isnanf.c.o
│   │   │   │       ├── s_isnanf.c.o.d
│   │   │   │       ├── s_modf.c.o
│   │   │   │       ├── s_modf.c.o.d
│   │   │   │       ├── s_scalbn.c.o
│   │   │   │       ├── s_scalbn.c.o.d
│   │   │   │       ├── s_sin.c.o
│   │   │   │       ├── s_sin.c.o.d
│   │   │   │       ├── s_tan.c.o
│   │   │   │       └── s_tan.c.o.d
│   │   │   ├── build.make
│   │   │   ├── cmake_clean_target.cmake
│   │   │   ├── cmake_clean.cmake
│   │   │   ├── compiler_depend.internal
│   │   │   ├── compiler_depend.make
│   │   │   ├── compiler_depend.ts
│   │   │   ├── depend.make
│   │   │   ├── DependInfo.cmake
│   │   │   ├── flags.make
│   │   │   ├── link.txt
│   │   │   └── progress.make
│   │   ├── SDL3_test.dir/
│   │   │   ├── src/
│   │   │   │   └── test/
│   │   │   │       ├── SDL_test_assert.c.o
│   │   │   │       ├── SDL_test_assert.c.o.d
│   │   │   │       ├── SDL_test_common.c.o
│   │   │   │       ├── SDL_test_common.c.o.d
│   │   │   │       ├── SDL_test_compare.c.o
│   │   │   │       ├── SDL_test_compare.c.o.d
│   │   │   │       ├── SDL_test_crc32.c.o
│   │   │   │       ├── SDL_test_crc32.c.o.d
│   │   │   │       ├── SDL_test_font.c.o
│   │   │   │       ├── SDL_test_font.c.o.d
│   │   │   │       ├── SDL_test_fuzzer.c.o
│   │   │   │       ├── SDL_test_fuzzer.c.o.d
│   │   │   │       ├── SDL_test_harness.c.o
│   │   │   │       ├── SDL_test_harness.c.o.d
│   │   │   │       ├── SDL_test_log.c.o
│   │   │   │       ├── SDL_test_log.c.o.d
│   │   │   │       ├── SDL_test_md5.c.o
│   │   │   │       ├── SDL_test_md5.c.o.d
│   │   │   │       ├── SDL_test_memory.c.o
│   │   │   │       └── SDL_test_memory.c.o.d
│   │   │   ├── build.make
│   │   │   ├── cmake_clean_target.cmake
│   │   │   ├── cmake_clean.cmake
│   │   │   ├── compiler_depend.internal
│   │   │   ├── compiler_depend.make
│   │   │   ├── compiler_depend.ts
│   │   │   ├── depend.make
│   │   │   ├── DependInfo.cmake
│   │   │   ├── flags.make
│   │   │   ├── link.txt
│   │   │   └── progress.make
│   │   ├── SDL3-shared.dir/
│   │   │   ├── src/
│   │   │   │   ├── atomic/
│   │   │   │   │   ├── SDL_atomic.c.o
│   │   │   │   │   ├── SDL_atomic.c.o.d
│   │   │   │   │   ├── SDL_spinlock.c.o
│   │   │   │   │   └── SDL_spinlock.c.o.d
│   │   │   │   ├── audio/
│   │   │   │   │   ├── alsa/
│   │   │   │   │   │   ├── SDL_alsa_audio.c.o
│   │   │   │   │   │   └── SDL_alsa_audio.c.o.d
│   │   │   │   │   ├── disk/
│   │   │   │   │   │   ├── SDL_diskaudio.c.o
│   │   │   │   │   │   └── SDL_diskaudio.c.o.d
│   │   │   │   │   ├── dummy/
│   │   │   │   │   │   ├── SDL_dummyaudio.c.o
│   │   │   │   │   │   └── SDL_dummyaudio.c.o.d
│   │   │   │   │   ├── pipewire/
│   │   │   │   │   │   ├── SDL_pipewire.c.o
│   │   │   │   │   │   └── SDL_pipewire.c.o.d
│   │   │   │   │   ├── pulseaudio/
│   │   │   │   │   │   ├── SDL_pulseaudio.c.o
│   │   │   │   │   │   └── SDL_pulseaudio.c.o.d
│   │   │   │   │   ├── SDL_audio.c.o
│   │   │   │   │   ├── SDL_audio.c.o.d
│   │   │   │   │   ├── SDL_audiocvt.c.o
│   │   │   │   │   ├── SDL_audiocvt.c.o.d
│   │   │   │   │   ├── SDL_audiodev.c.o
│   │   │   │   │   ├── SDL_audiodev.c.o.d
│   │   │   │   │   ├── SDL_audioqueue.c.o
│   │   │   │   │   ├── SDL_audioqueue.c.o.d
│   │   │   │   │   ├── SDL_audioresample.c.o
│   │   │   │   │   ├── SDL_audioresample.c.o.d
│   │   │   │   │   ├── SDL_audiotypecvt.c.o
│   │   │   │   │   ├── SDL_audiotypecvt.c.o.d
│   │   │   │   │   ├── SDL_mixer.c.o
│   │   │   │   │   ├── SDL_mixer.c.o.d
│   │   │   │   │   ├── SDL_wave.c.o
│   │   │   │   │   └── SDL_wave.c.o.d
│   │   │   │   ├── camera/
│   │   │   │   │   ├── dummy/
│   │   │   │   │   │   ├── SDL_camera_dummy.c.o
│   │   │   │   │   │   └── SDL_camera_dummy.c.o.d
│   │   │   │   │   ├── pipewire/
│   │   │   │   │   │   ├── SDL_camera_pipewire.c.o
│   │   │   │   │   │   └── SDL_camera_pipewire.c.o.d
│   │   │   │   │   ├── v4l2/
│   │   │   │   │   │   ├── SDL_camera_v4l2.c.o
│   │   │   │   │   │   └── SDL_camera_v4l2.c.o.d
│   │   │   │   │   ├── SDL_camera.c.o
│   │   │   │   │   └── SDL_camera.c.o.d
│   │   │   │   ├── core/
│   │   │   │   │   ├── linux/
│   │   │   │   │   │   ├── SDL_dbus.c.o
│   │   │   │   │   │   ├── SDL_dbus.c.o.d
│   │   │   │   │   │   ├── SDL_evdev_capabilities.c.o
│   │   │   │   │   │   ├── SDL_evdev_capabilities.c.o.d
│   │   │   │   │   │   ├── SDL_evdev_kbd.c.o
│   │   │   │   │   │   ├── SDL_evdev_kbd.c.o.d
│   │   │   │   │   │   ├── SDL_evdev.c.o
│   │   │   │   │   │   ├── SDL_evdev.c.o.d
│   │   │   │   │   │   ├── SDL_fcitx.c.o
│   │   │   │   │   │   ├── SDL_fcitx.c.o.d
│   │   │   │   │   │   ├── SDL_ibus.c.o
│   │   │   │   │   │   ├── SDL_ibus.c.o.d
│   │   │   │   │   │   ├── SDL_ime.c.o
│   │   │   │   │   │   ├── SDL_ime.c.o.d
│   │   │   │   │   │   ├── SDL_progressbar.c.o
│   │   │   │   │   │   ├── SDL_progressbar.c.o.d
│   │   │   │   │   │   ├── SDL_system_theme.c.o
│   │   │   │   │   │   ├── SDL_system_theme.c.o.d
│   │   │   │   │   │   ├── SDL_threadprio.c.o
│   │   │   │   │   │   ├── SDL_threadprio.c.o.d
│   │   │   │   │   │   ├── SDL_ubuntu_touch.c.o
│   │   │   │   │   │   ├── SDL_ubuntu_touch.c.o.d
│   │   │   │   │   │   ├── SDL_udev.c.o
│   │   │   │   │   │   └── SDL_udev.c.o.d
│   │   │   │   │   ├── unix/
│   │   │   │   │   │   ├── SDL_appid.c.o
│   │   │   │   │   │   ├── SDL_appid.c.o.d
│   │   │   │   │   │   ├── SDL_fribidi.c.o
│   │   │   │   │   │   ├── SDL_fribidi.c.o.d
│   │   │   │   │   │   ├── SDL_libthai.c.o
│   │   │   │   │   │   ├── SDL_libthai.c.o.d
│   │   │   │   │   │   ├── SDL_poll.c.o
│   │   │   │   │   │   └── SDL_poll.c.o.d
│   │   │   │   │   ├── SDL_core_unsupported.c.o
│   │   │   │   │   └── SDL_core_unsupported.c.o.d
│   │   │   │   ├── cpuinfo/
│   │   │   │   │   ├── SDL_cpuinfo.c.o
│   │   │   │   │   └── SDL_cpuinfo.c.o.d
│   │   │   │   ├── dialog/
│   │   │   │   │   ├── unix/
│   │   │   │   │   │   ├── SDL_portaldialog.c.o
│   │   │   │   │   │   ├── SDL_portaldialog.c.o.d
│   │   │   │   │   │   ├── SDL_unixdialog.c.o
│   │   │   │   │   │   ├── SDL_unixdialog.c.o.d
│   │   │   │   │   │   ├── SDL_zenitydialog.c.o
│   │   │   │   │   │   ├── SDL_zenitydialog.c.o.d
│   │   │   │   │   │   ├── SDL_zenitymessagebox.c.o
│   │   │   │   │   │   └── SDL_zenitymessagebox.c.o.d
│   │   │   │   │   ├── SDL_dialog_utils.c.o
│   │   │   │   │   ├── SDL_dialog_utils.c.o.d
│   │   │   │   │   ├── SDL_dialog.c.o
│   │   │   │   │   └── SDL_dialog.c.o.d
│   │   │   │   ├── dynapi/
│   │   │   │   │   ├── SDL_dynapi.c.o
│   │   │   │   │   └── SDL_dynapi.c.o.d
│   │   │   │   ├── events/
│   │   │   │   │   ├── imKStoUCS.c.o
│   │   │   │   │   ├── imKStoUCS.c.o.d
│   │   │   │   │   ├── SDL_categories.c.o
│   │   │   │   │   ├── SDL_categories.c.o.d
│   │   │   │   │   ├── SDL_clipboardevents.c.o
│   │   │   │   │   ├── SDL_clipboardevents.c.o.d
│   │   │   │   │   ├── SDL_displayevents.c.o
│   │   │   │   │   ├── SDL_displayevents.c.o.d
│   │   │   │   │   ├── SDL_dropevents.c.o
│   │   │   │   │   ├── SDL_dropevents.c.o.d
│   │   │   │   │   ├── SDL_events.c.o
│   │   │   │   │   ├── SDL_events.c.o.d
│   │   │   │   │   ├── SDL_eventwatch.c.o
│   │   │   │   │   ├── SDL_eventwatch.c.o.d
│   │   │   │   │   ├── SDL_keyboard.c.o
│   │   │   │   │   ├── SDL_keyboard.c.o.d
│   │   │   │   │   ├── SDL_keymap.c.o
│   │   │   │   │   ├── SDL_keymap.c.o.d
│   │   │   │   │   ├── SDL_keysym_to_keycode.c.o
│   │   │   │   │   ├── SDL_keysym_to_keycode.c.o.d
│   │   │   │   │   ├── SDL_keysym_to_scancode.c.o
│   │   │   │   │   ├── SDL_keysym_to_scancode.c.o.d
│   │   │   │   │   ├── SDL_mouse.c.o
│   │   │   │   │   ├── SDL_mouse.c.o.d
│   │   │   │   │   ├── SDL_notificationevents.c.o
│   │   │   │   │   ├── SDL_notificationevents.c.o.d
│   │   │   │   │   ├── SDL_pen.c.o
│   │   │   │   │   ├── SDL_pen.c.o.d
│   │   │   │   │   ├── SDL_quit.c.o
│   │   │   │   │   ├── SDL_quit.c.o.d
│   │   │   │   │   ├── SDL_scancode_tables.c.o
│   │   │   │   │   ├── SDL_scancode_tables.c.o.d
│   │   │   │   │   ├── SDL_touch.c.o
│   │   │   │   │   ├── SDL_touch.c.o.d
│   │   │   │   │   ├── SDL_windowevents.c.o
│   │   │   │   │   └── SDL_windowevents.c.o.d
│   │   │   │   ├── filesystem/
│   │   │   │   │   ├── posix/
│   │   │   │   │   │   ├── SDL_sysfsops.c.o
│   │   │   │   │   │   └── SDL_sysfsops.c.o.d
│   │   │   │   │   ├── unix/
│   │   │   │   │   │   ├── SDL_sysfilesystem.c.o
│   │   │   │   │   │   └── SDL_sysfilesystem.c.o.d
│   │   │   │   │   ├── SDL_filesystem.c.o
│   │   │   │   │   └── SDL_filesystem.c.o.d
│   │   │   │   ├── gpu/
│   │   │   │   │   ├── vulkan/
│   │   │   │   │   │   ├── SDL_gpu_vulkan.c.o
│   │   │   │   │   │   └── SDL_gpu_vulkan.c.o.d
│   │   │   │   │   ├── xr/
│   │   │   │   │   │   ├── SDL_gpu_openxr.c.o
│   │   │   │   │   │   ├── SDL_gpu_openxr.c.o.d
│   │   │   │   │   │   ├── SDL_openxrdyn.c.o
│   │   │   │   │   │   └── SDL_openxrdyn.c.o.d
│   │   │   │   │   ├── SDL_gpu.c.o
│   │   │   │   │   └── SDL_gpu.c.o.d
│   │   │   │   ├── haptic/
│   │   │   │   │   ├── hidapi/
│   │   │   │   │   │   ├── SDL_hidapihaptic_lg4ff.c.o
│   │   │   │   │   │   ├── SDL_hidapihaptic_lg4ff.c.o.d
│   │   │   │   │   │   ├── SDL_hidapihaptic.c.o
│   │   │   │   │   │   └── SDL_hidapihaptic.c.o.d
│   │   │   │   │   ├── linux/
│   │   │   │   │   │   ├── SDL_syshaptic.c.o
│   │   │   │   │   │   └── SDL_syshaptic.c.o.d
│   │   │   │   │   ├── SDL_haptic.c.o
│   │   │   │   │   └── SDL_haptic.c.o.d
│   │   │   │   ├── hidapi/
│   │   │   │   │   ├── SDL_hidapi.c.o
│   │   │   │   │   └── SDL_hidapi.c.o.d
│   │   │   │   ├── io/
│   │   │   │   │   ├── generic/
│   │   │   │   │   │   ├── SDL_asyncio_generic.c.o
│   │   │   │   │   │   └── SDL_asyncio_generic.c.o.d
│   │   │   │   │   ├── io_uring/
│   │   │   │   │   │   ├── SDL_asyncio_liburing.c.o
│   │   │   │   │   │   └── SDL_asyncio_liburing.c.o.d
│   │   │   │   │   ├── SDL_asyncio.c.o
│   │   │   │   │   ├── SDL_asyncio.c.o.d
│   │   │   │   │   ├── SDL_iostream.c.o
│   │   │   │   │   └── SDL_iostream.c.o.d
│   │   │   │   ├── joystick/
│   │   │   │   │   ├── hidapi/
│   │   │   │   │   │   ├── SDL_hidapi_8bitdo.c.o
│   │   │   │   │   │   ├── SDL_hidapi_8bitdo.c.o.d
│   │   │   │   │   │   ├── SDL_hidapi_combined.c.o
│   │   │   │   │   │   ├── SDL_hidapi_combined.c.o.d
│   │   │   │   │   │   ├── SDL_hidapi_flydigi.c.o
│   │   │   │   │   │   ├── SDL_hidapi_flydigi.c.o.d
│   │   │   │   │   │   ├── SDL_hidapi_gamecube.c.o
│   │   │   │   │   │   ├── SDL_hidapi_gamecube.c.o.d
│   │   │   │   │   │   ├── SDL_hidapi_gamesir.c.o
│   │   │   │   │   │   ├── SDL_hidapi_gamesir.c.o.d
│   │   │   │   │   │   ├── SDL_hidapi_gip.c.o
│   │   │   │   │   │   ├── SDL_hidapi_gip.c.o.d
│   │   │   │   │   │   ├── SDL_hidapi_lg4ff.c.o
│   │   │   │   │   │   ├── SDL_hidapi_lg4ff.c.o.d
│   │   │   │   │   │   ├── SDL_hidapi_luna.c.o
│   │   │   │   │   │   ├── SDL_hidapi_luna.c.o.d
│   │   │   │   │   │   ├── SDL_hidapi_ps3.c.o
│   │   │   │   │   │   ├── SDL_hidapi_ps3.c.o.d
│   │   │   │   │   │   ├── SDL_hidapi_ps4.c.o
│   │   │   │   │   │   ├── SDL_hidapi_ps4.c.o.d
│   │   │   │   │   │   ├── SDL_hidapi_ps5.c.o
│   │   │   │   │   │   ├── SDL_hidapi_ps5.c.o.d
│   │   │   │   │   │   ├── SDL_hidapi_rumble.c.o
│   │   │   │   │   │   ├── SDL_hidapi_rumble.c.o.d
│   │   │   │   │   │   ├── SDL_hidapi_shield.c.o
│   │   │   │   │   │   ├── SDL_hidapi_shield.c.o.d
│   │   │   │   │   │   ├── SDL_hidapi_sinput.c.o
│   │   │   │   │   │   ├── SDL_hidapi_sinput.c.o.d
│   │   │   │   │   │   ├── SDL_hidapi_stadia.c.o
│   │   │   │   │   │   ├── SDL_hidapi_stadia.c.o.d
│   │   │   │   │   │   ├── SDL_hidapi_steam_hori.c.o
│   │   │   │   │   │   ├── SDL_hidapi_steam_hori.c.o.d
│   │   │   │   │   │   ├── SDL_hidapi_steam_triton.c.o
│   │   │   │   │   │   ├── SDL_hidapi_steam_triton.c.o.d
│   │   │   │   │   │   ├── SDL_hidapi_steam.c.o
│   │   │   │   │   │   ├── SDL_hidapi_steam.c.o.d
│   │   │   │   │   │   ├── SDL_hidapi_steamdeck.c.o
│   │   │   │   │   │   ├── SDL_hidapi_steamdeck.c.o.d
│   │   │   │   │   │   ├── SDL_hidapi_switch.c.o
│   │   │   │   │   │   ├── SDL_hidapi_switch.c.o.d
│   │   │   │   │   │   ├── SDL_hidapi_switch2.c.o
│   │   │   │   │   │   ├── SDL_hidapi_switch2.c.o.d
│   │   │   │   │   │   ├── SDL_hidapi_wii.c.o
│   │   │   │   │   │   ├── SDL_hidapi_wii.c.o.d
│   │   │   │   │   │   ├── SDL_hidapi_xbox360.c.o
│   │   │   │   │   │   ├── SDL_hidapi_xbox360.c.o.d
│   │   │   │   │   │   ├── SDL_hidapi_xbox360bb.c.o
│   │   │   │   │   │   ├── SDL_hidapi_xbox360bb.c.o.d
│   │   │   │   │   │   ├── SDL_hidapi_xbox360w.c.o
│   │   │   │   │   │   ├── SDL_hidapi_xbox360w.c.o.d
│   │   │   │   │   │   ├── SDL_hidapi_xboxone.c.o
│   │   │   │   │   │   ├── SDL_hidapi_xboxone.c.o.d
│   │   │   │   │   │   ├── SDL_hidapi_zuiki.c.o
│   │   │   │   │   │   ├── SDL_hidapi_zuiki.c.o.d
│   │   │   │   │   │   ├── SDL_hidapijoystick.c.o
│   │   │   │   │   │   ├── SDL_hidapijoystick.c.o.d
│   │   │   │   │   │   ├── SDL_report_descriptor.c.o
│   │   │   │   │   │   └── SDL_report_descriptor.c.o.d
│   │   │   │   │   ├── linux/
│   │   │   │   │   │   ├── SDL_sysjoystick.c.o
│   │   │   │   │   │   └── SDL_sysjoystick.c.o.d
│   │   │   │   │   ├── virtual/
│   │   │   │   │   │   ├── SDL_virtualjoystick.c.o
│   │   │   │   │   │   └── SDL_virtualjoystick.c.o.d
│   │   │   │   │   ├── controller_type.c.o
│   │   │   │   │   ├── controller_type.c.o.d
│   │   │   │   │   ├── SDL_gamepad.c.o
│   │   │   │   │   ├── SDL_gamepad.c.o.d
│   │   │   │   │   ├── SDL_joystick.c.o
│   │   │   │   │   ├── SDL_joystick.c.o.d
│   │   │   │   │   ├── SDL_steam_virtual_gamepad.c.o
│   │   │   │   │   └── SDL_steam_virtual_gamepad.c.o.d
│   │   │   │   ├── loadso/
│   │   │   │   │   └── dlopen/
│   │   │   │   │       ├── SDL_sysloadso.c.o
│   │   │   │   │       └── SDL_sysloadso.c.o.d
│   │   │   │   ├── locale/
│   │   │   │   │   ├── unix/
│   │   │   │   │   │   ├── SDL_syslocale.c.o
│   │   │   │   │   │   └── SDL_syslocale.c.o.d
│   │   │   │   │   ├── SDL_locale.c.o
│   │   │   │   │   └── SDL_locale.c.o.d
│   │   │   │   ├── main/
│   │   │   │   │   ├── generic/
│   │   │   │   │   │   ├── SDL_sysmain_callbacks.c.o
│   │   │   │   │   │   └── SDL_sysmain_callbacks.c.o.d
│   │   │   │   │   ├── SDL_main_callbacks.c.o
│   │   │   │   │   ├── SDL_main_callbacks.c.o.d
│   │   │   │   │   ├── SDL_runapp.c.o
│   │   │   │   │   └── SDL_runapp.c.o.d
│   │   │   │   ├── misc/
│   │   │   │   │   ├── unix/
│   │   │   │   │   │   ├── SDL_sysurl.c.o
│   │   │   │   │   │   └── SDL_sysurl.c.o.d
│   │   │   │   │   ├── SDL_libusb.c.o
│   │   │   │   │   ├── SDL_libusb.c.o.d
│   │   │   │   │   ├── SDL_url.c.o
│   │   │   │   │   └── SDL_url.c.o.d
│   │   │   │   ├── notification/
│   │   │   │   │   ├── unix/
│   │   │   │   │   │   ├── SDL_dbusnotification.c.o
│   │   │   │   │   │   └── SDL_dbusnotification.c.o.d
│   │   │   │   │   ├── SDL_notification.c.o
│   │   │   │   │   └── SDL_notification.c.o.d
│   │   │   │   ├── power/
│   │   │   │   │   ├── linux/
│   │   │   │   │   │   ├── SDL_syspower.c.o
│   │   │   │   │   │   └── SDL_syspower.c.o.d
│   │   │   │   │   ├── SDL_power.c.o
│   │   │   │   │   └── SDL_power.c.o.d
│   │   │   │   ├── process/
│   │   │   │   │   ├── posix/
│   │   │   │   │   │   ├── SDL_posixprocess.c.o
│   │   │   │   │   │   └── SDL_posixprocess.c.o.d
│   │   │   │   │   ├── SDL_process.c.o
│   │   │   │   │   └── SDL_process.c.o.d
│   │   │   │   ├── render/
│   │   │   │   │   ├── direct3d/
│   │   │   │   │   │   ├── SDL_render_d3d.c.o
│   │   │   │   │   │   ├── SDL_render_d3d.c.o.d
│   │   │   │   │   │   ├── SDL_shaders_d3d.c.o
│   │   │   │   │   │   └── SDL_shaders_d3d.c.o.d
│   │   │   │   │   ├── direct3d11/
│   │   │   │   │   │   ├── SDL_render_d3d11.c.o
│   │   │   │   │   │   ├── SDL_render_d3d11.c.o.d
│   │   │   │   │   │   ├── SDL_shaders_d3d11.c.o
│   │   │   │   │   │   └── SDL_shaders_d3d11.c.o.d
│   │   │   │   │   ├── direct3d12/
│   │   │   │   │   │   ├── SDL_render_d3d12.c.o
│   │   │   │   │   │   ├── SDL_render_d3d12.c.o.d
│   │   │   │   │   │   ├── SDL_shaders_d3d12.c.o
│   │   │   │   │   │   └── SDL_shaders_d3d12.c.o.d
│   │   │   │   │   ├── gpu/
│   │   │   │   │   │   ├── SDL_pipeline_gpu.c.o
│   │   │   │   │   │   ├── SDL_pipeline_gpu.c.o.d
│   │   │   │   │   │   ├── SDL_render_gpu.c.o
│   │   │   │   │   │   ├── SDL_render_gpu.c.o.d
│   │   │   │   │   │   ├── SDL_shaders_gpu.c.o
│   │   │   │   │   │   └── SDL_shaders_gpu.c.o.d
│   │   │   │   │   ├── ngage/
│   │   │   │   │   │   ├── SDL_render_ngage.c.o
│   │   │   │   │   │   └── SDL_render_ngage.c.o.d
│   │   │   │   │   ├── opengl/
│   │   │   │   │   │   ├── SDL_render_gl.c.o
│   │   │   │   │   │   ├── SDL_render_gl.c.o.d
│   │   │   │   │   │   ├── SDL_shaders_gl.c.o
│   │   │   │   │   │   └── SDL_shaders_gl.c.o.d
│   │   │   │   │   ├── opengles/
│   │   │   │   │   │   ├── SDL_render_gles.c.o
│   │   │   │   │   │   └── SDL_render_gles.c.o.d
│   │   │   │   │   ├── opengles2/
│   │   │   │   │   │   ├── SDL_render_gles2.c.o
│   │   │   │   │   │   ├── SDL_render_gles2.c.o.d
│   │   │   │   │   │   ├── SDL_shaders_gles2.c.o
│   │   │   │   │   │   └── SDL_shaders_gles2.c.o.d
│   │   │   │   │   ├── ps2/
│   │   │   │   │   │   ├── SDL_render_ps2.c.o
│   │   │   │   │   │   └── SDL_render_ps2.c.o.d
│   │   │   │   │   ├── psp/
│   │   │   │   │   │   ├── SDL_render_psp.c.o
│   │   │   │   │   │   └── SDL_render_psp.c.o.d
│   │   │   │   │   ├── software/
│   │   │   │   │   │   ├── SDL_blendfillrect.c.o
│   │   │   │   │   │   ├── SDL_blendfillrect.c.o.d
│   │   │   │   │   │   ├── SDL_blendline.c.o
│   │   │   │   │   │   ├── SDL_blendline.c.o.d
│   │   │   │   │   │   ├── SDL_blendpoint.c.o
│   │   │   │   │   │   ├── SDL_blendpoint.c.o.d
│   │   │   │   │   │   ├── SDL_drawline.c.o
│   │   │   │   │   │   ├── SDL_drawline.c.o.d
│   │   │   │   │   │   ├── SDL_drawpoint.c.o
│   │   │   │   │   │   ├── SDL_drawpoint.c.o.d
│   │   │   │   │   │   ├── SDL_render_sw.c.o
│   │   │   │   │   │   ├── SDL_render_sw.c.o.d
│   │   │   │   │   │   ├── SDL_triangle.c.o
│   │   │   │   │   │   └── SDL_triangle.c.o.d
│   │   │   │   │   ├── vitagxm/
│   │   │   │   │   │   ├── SDL_render_vita_gxm_memory.c.o
│   │   │   │   │   │   ├── SDL_render_vita_gxm_memory.c.o.d
│   │   │   │   │   │   ├── SDL_render_vita_gxm_tools.c.o
│   │   │   │   │   │   ├── SDL_render_vita_gxm_tools.c.o.d
│   │   │   │   │   │   ├── SDL_render_vita_gxm.c.o
│   │   │   │   │   │   └── SDL_render_vita_gxm.c.o.d
│   │   │   │   │   ├── vulkan/
│   │   │   │   │   │   ├── SDL_render_vulkan.c.o
│   │   │   │   │   │   ├── SDL_render_vulkan.c.o.d
│   │   │   │   │   │   ├── SDL_shaders_vulkan.c.o
│   │   │   │   │   │   └── SDL_shaders_vulkan.c.o.d
│   │   │   │   │   ├── SDL_render_unsupported.c.o
│   │   │   │   │   ├── SDL_render_unsupported.c.o.d
│   │   │   │   │   ├── SDL_render.c.o
│   │   │   │   │   ├── SDL_render.c.o.d
│   │   │   │   │   ├── SDL_yuv_sw.c.o
│   │   │   │   │   └── SDL_yuv_sw.c.o.d
│   │   │   │   ├── sensor/
│   │   │   │   │   ├── dummy/
│   │   │   │   │   │   ├── SDL_dummysensor.c.o
│   │   │   │   │   │   └── SDL_dummysensor.c.o.d
│   │   │   │   │   ├── SDL_sensor.c.o
│   │   │   │   │   └── SDL_sensor.c.o.d
│   │   │   │   ├── stdlib/
│   │   │   │   │   ├── SDL_crc16.c.o
│   │   │   │   │   ├── SDL_crc16.c.o.d
│   │   │   │   │   ├── SDL_crc32.c.o
│   │   │   │   │   ├── SDL_crc32.c.o.d
│   │   │   │   │   ├── SDL_getenv.c.o
│   │   │   │   │   ├── SDL_getenv.c.o.d
│   │   │   │   │   ├── SDL_iconv.c.o
│   │   │   │   │   ├── SDL_iconv.c.o.d
│   │   │   │   │   ├── SDL_malloc.c.o
│   │   │   │   │   ├── SDL_malloc.c.o.d
│   │   │   │   │   ├── SDL_memcpy.c.o
│   │   │   │   │   ├── SDL_memcpy.c.o.d
│   │   │   │   │   ├── SDL_memmove.c.o
│   │   │   │   │   ├── SDL_memmove.c.o.d
│   │   │   │   │   ├── SDL_memset.c.o
│   │   │   │   │   ├── SDL_memset.c.o.d
│   │   │   │   │   ├── SDL_mslibc.c.o
│   │   │   │   │   ├── SDL_mslibc.c.o.d
│   │   │   │   │   ├── SDL_murmur3.c.o
│   │   │   │   │   ├── SDL_murmur3.c.o.d
│   │   │   │   │   ├── SDL_qsort.c.o
│   │   │   │   │   ├── SDL_qsort.c.o.d
│   │   │   │   │   ├── SDL_random.c.o
│   │   │   │   │   ├── SDL_random.c.o.d
│   │   │   │   │   ├── SDL_stdlib.c.o
│   │   │   │   │   ├── SDL_stdlib.c.o.d
│   │   │   │   │   ├── SDL_string.c.o
│   │   │   │   │   ├── SDL_string.c.o.d
│   │   │   │   │   ├── SDL_strtokr.c.o
│   │   │   │   │   └── SDL_strtokr.c.o.d
│   │   │   │   ├── storage/
│   │   │   │   │   ├── generic/
│   │   │   │   │   │   ├── SDL_genericstorage.c.o
│   │   │   │   │   │   └── SDL_genericstorage.c.o.d
│   │   │   │   │   ├── steam/
│   │   │   │   │   │   ├── SDL_steamstorage.c.o
│   │   │   │   │   │   └── SDL_steamstorage.c.o.d
│   │   │   │   │   ├── SDL_storage.c.o
│   │   │   │   │   └── SDL_storage.c.o.d
│   │   │   │   ├── thread/
│   │   │   │   │   ├── pthread/
│   │   │   │   │   │   ├── SDL_syscond.c.o
│   │   │   │   │   │   ├── SDL_syscond.c.o.d
│   │   │   │   │   │   ├── SDL_sysmutex.c.o
│   │   │   │   │   │   ├── SDL_sysmutex.c.o.d
│   │   │   │   │   │   ├── SDL_sysrwlock.c.o
│   │   │   │   │   │   ├── SDL_sysrwlock.c.o.d
│   │   │   │   │   │   ├── SDL_syssem.c.o
│   │   │   │   │   │   ├── SDL_syssem.c.o.d
│   │   │   │   │   │   ├── SDL_systhread.c.o
│   │   │   │   │   │   ├── SDL_systhread.c.o.d
│   │   │   │   │   │   ├── SDL_systls.c.o
│   │   │   │   │   │   └── SDL_systls.c.o.d
│   │   │   │   │   ├── SDL_thread.c.o
│   │   │   │   │   └── SDL_thread.c.o.d
│   │   │   │   ├── time/
│   │   │   │   │   ├── unix/
│   │   │   │   │   │   ├── SDL_systime.c.o
│   │   │   │   │   │   └── SDL_systime.c.o.d
│   │   │   │   │   ├── SDL_time.c.o
│   │   │   │   │   └── SDL_time.c.o.d
│   │   │   │   ├── timer/
│   │   │   │   │   ├── unix/
│   │   │   │   │   │   ├── SDL_systimer.c.o
│   │   │   │   │   │   └── SDL_systimer.c.o.d
│   │   │   │   │   ├── SDL_timer.c.o
│   │   │   │   │   └── SDL_timer.c.o.d
│   │   │   │   ├── tray/
│   │   │   │   │   ├── unix/
│   │   │   │   │   │   ├── SDL_dbustray.c.o
│   │   │   │   │   │   ├── SDL_dbustray.c.o.d
│   │   │   │   │   │   ├── SDL_tray.c.o
│   │   │   │   │   │   └── SDL_tray.c.o.d
│   │   │   │   │   ├── SDL_tray_utils.c.o
│   │   │   │   │   └── SDL_tray_utils.c.o.d
│   │   │   │   ├── video/
│   │   │   │   │   ├── dummy/
│   │   │   │   │   │   ├── SDL_nullevents.c.o
│   │   │   │   │   │   ├── SDL_nullevents.c.o.d
│   │   │   │   │   │   ├── SDL_nullframebuffer.c.o
│   │   │   │   │   │   ├── SDL_nullframebuffer.c.o.d
│   │   │   │   │   │   ├── SDL_nullvideo.c.o
│   │   │   │   │   │   └── SDL_nullvideo.c.o.d
│   │   │   │   │   ├── kmsdrm/
│   │   │   │   │   │   ├── SDL_kmsdrmdyn.c.o
│   │   │   │   │   │   ├── SDL_kmsdrmdyn.c.o.d
│   │   │   │   │   │   ├── SDL_kmsdrmevents.c.o
│   │   │   │   │   │   ├── SDL_kmsdrmevents.c.o.d
│   │   │   │   │   │   ├── SDL_kmsdrmmouse.c.o
│   │   │   │   │   │   ├── SDL_kmsdrmmouse.c.o.d
│   │   │   │   │   │   ├── SDL_kmsdrmopengles.c.o
│   │   │   │   │   │   ├── SDL_kmsdrmopengles.c.o.d
│   │   │   │   │   │   ├── SDL_kmsdrmvideo.c.o
│   │   │   │   │   │   ├── SDL_kmsdrmvideo.c.o.d
│   │   │   │   │   │   ├── SDL_kmsdrmvulkan.c.o
│   │   │   │   │   │   └── SDL_kmsdrmvulkan.c.o.d
│   │   │   │   │   ├── offscreen/
│   │   │   │   │   │   ├── SDL_offscreenevents.c.o
│   │   │   │   │   │   ├── SDL_offscreenevents.c.o.d
│   │   │   │   │   │   ├── SDL_offscreenframebuffer.c.o
│   │   │   │   │   │   ├── SDL_offscreenframebuffer.c.o.d
│   │   │   │   │   │   ├── SDL_offscreenopengles.c.o
│   │   │   │   │   │   ├── SDL_offscreenopengles.c.o.d
│   │   │   │   │   │   ├── SDL_offscreenvideo.c.o
│   │   │   │   │   │   ├── SDL_offscreenvideo.c.o.d
│   │   │   │   │   │   ├── SDL_offscreenvulkan.c.o
│   │   │   │   │   │   ├── SDL_offscreenvulkan.c.o.d
│   │   │   │   │   │   ├── SDL_offscreenwindow.c.o
│   │   │   │   │   │   └── SDL_offscreenwindow.c.o.d
│   │   │   │   │   ├── wayland/
│   │   │   │   │   │   ├── SDL_waylandclipboard.c.o
│   │   │   │   │   │   ├── SDL_waylandclipboard.c.o.d
│   │   │   │   │   │   ├── SDL_waylandcolor.c.o
│   │   │   │   │   │   ├── SDL_waylandcolor.c.o.d
│   │   │   │   │   │   ├── SDL_waylanddatamanager.c.o
│   │   │   │   │   │   ├── SDL_waylanddatamanager.c.o.d
│   │   │   │   │   │   ├── SDL_waylanddyn.c.o
│   │   │   │   │   │   ├── SDL_waylanddyn.c.o.d
│   │   │   │   │   │   ├── SDL_waylandevents.c.o
│   │   │   │   │   │   ├── SDL_waylandevents.c.o.d
│   │   │   │   │   │   ├── SDL_waylandeventthread.c.o
│   │   │   │   │   │   ├── SDL_waylandeventthread.c.o.d
│   │   │   │   │   │   ├── SDL_waylandkeyboard.c.o
│   │   │   │   │   │   ├── SDL_waylandkeyboard.c.o.d
│   │   │   │   │   │   ├── SDL_waylandmessagebox.c.o
│   │   │   │   │   │   ├── SDL_waylandmessagebox.c.o.d
│   │   │   │   │   │   ├── SDL_waylandmouse.c.o
│   │   │   │   │   │   ├── SDL_waylandmouse.c.o.d
│   │   │   │   │   │   ├── SDL_waylandopengles.c.o
│   │   │   │   │   │   ├── SDL_waylandopengles.c.o.d
│   │   │   │   │   │   ├── SDL_waylandshmbuffer.c.o
│   │   │   │   │   │   ├── SDL_waylandshmbuffer.c.o.d
│   │   │   │   │   │   ├── SDL_waylandutil.c.o
│   │   │   │   │   │   ├── SDL_waylandutil.c.o.d
│   │   │   │   │   │   ├── SDL_waylandvideo.c.o
│   │   │   │   │   │   ├── SDL_waylandvideo.c.o.d
│   │   │   │   │   │   ├── SDL_waylandvulkan.c.o
│   │   │   │   │   │   ├── SDL_waylandvulkan.c.o.d
│   │   │   │   │   │   ├── SDL_waylandwindow.c.o
│   │   │   │   │   │   └── SDL_waylandwindow.c.o.d
│   │   │   │   │   ├── x11/
│   │   │   │   │   │   ├── edid-parse.c.o
│   │   │   │   │   │   ├── edid-parse.c.o.d
│   │   │   │   │   │   ├── SDL_x11clipboard.c.o
│   │   │   │   │   │   ├── SDL_x11clipboard.c.o.d
│   │   │   │   │   │   ├── SDL_x11dyn.c.o
│   │   │   │   │   │   ├── SDL_x11dyn.c.o.d
│   │   │   │   │   │   ├── SDL_x11events.c.o
│   │   │   │   │   │   ├── SDL_x11events.c.o.d
│   │   │   │   │   │   ├── SDL_x11framebuffer.c.o
│   │   │   │   │   │   ├── SDL_x11framebuffer.c.o.d
│   │   │   │   │   │   ├── SDL_x11keyboard.c.o
│   │   │   │   │   │   ├── SDL_x11keyboard.c.o.d
│   │   │   │   │   │   ├── SDL_x11messagebox.c.o
│   │   │   │   │   │   ├── SDL_x11messagebox.c.o.d
│   │   │   │   │   │   ├── SDL_x11modes.c.o
│   │   │   │   │   │   ├── SDL_x11modes.c.o.d
│   │   │   │   │   │   ├── SDL_x11mouse.c.o
│   │   │   │   │   │   ├── SDL_x11mouse.c.o.d
│   │   │   │   │   │   ├── SDL_x11opengl.c.o
│   │   │   │   │   │   ├── SDL_x11opengl.c.o.d
│   │   │   │   │   │   ├── SDL_x11opengles.c.o
│   │   │   │   │   │   ├── SDL_x11opengles.c.o.d
│   │   │   │   │   │   ├── SDL_x11pen.c.o
│   │   │   │   │   │   ├── SDL_x11pen.c.o.d
│   │   │   │   │   │   ├── SDL_x11settings.c.o
│   │   │   │   │   │   ├── SDL_x11settings.c.o.d
│   │   │   │   │   │   ├── SDL_x11shape.c.o
│   │   │   │   │   │   ├── SDL_x11shape.c.o.d
│   │   │   │   │   │   ├── SDL_x11toolkit.c.o
│   │   │   │   │   │   ├── SDL_x11toolkit.c.o.d
│   │   │   │   │   │   ├── SDL_x11touch.c.o
│   │   │   │   │   │   ├── SDL_x11touch.c.o.d
│   │   │   │   │   │   ├── SDL_x11video.c.o
│   │   │   │   │   │   ├── SDL_x11video.c.o.d
│   │   │   │   │   │   ├── SDL_x11vulkan.c.o
│   │   │   │   │   │   ├── SDL_x11vulkan.c.o.d
│   │   │   │   │   │   ├── SDL_x11window.c.o
│   │   │   │   │   │   ├── SDL_x11window.c.o.d
│   │   │   │   │   │   ├── SDL_x11xfixes.c.o
│   │   │   │   │   │   ├── SDL_x11xfixes.c.o.d
│   │   │   │   │   │   ├── SDL_x11xinput2.c.o
│   │   │   │   │   │   ├── SDL_x11xinput2.c.o.d
│   │   │   │   │   │   ├── SDL_x11xsync.c.o
│   │   │   │   │   │   ├── SDL_x11xsync.c.o.d
│   │   │   │   │   │   ├── SDL_x11xtest.c.o
│   │   │   │   │   │   ├── SDL_x11xtest.c.o.d
│   │   │   │   │   │   ├── xsettings-client.c.o
│   │   │   │   │   │   └── xsettings-client.c.o.d
│   │   │   │   │   ├── yuv2rgb/
│   │   │   │   │   │   ├── yuv_rgb_lsx.c.o
│   │   │   │   │   │   ├── yuv_rgb_lsx.c.o.d
│   │   │   │   │   │   ├── yuv_rgb_sse.c.o
│   │   │   │   │   │   ├── yuv_rgb_sse.c.o.d
│   │   │   │   │   │   ├── yuv_rgb_std.c.o
│   │   │   │   │   │   └── yuv_rgb_std.c.o.d
│   │   │   │   │   ├── SDL_blit_0.c.o
│   │   │   │   │   ├── SDL_blit_0.c.o.d
│   │   │   │   │   ├── SDL_blit_1.c.o
│   │   │   │   │   ├── SDL_blit_1.c.o.d
│   │   │   │   │   ├── SDL_blit_A.c.o
│   │   │   │   │   ├── SDL_blit_A.c.o.d
│   │   │   │   │   ├── SDL_blit_auto.c.o
│   │   │   │   │   ├── SDL_blit_auto.c.o.d
│   │   │   │   │   ├── SDL_blit_copy.c.o
│   │   │   │   │   ├── SDL_blit_copy.c.o.d
│   │   │   │   │   ├── SDL_blit_N.c.o
│   │   │   │   │   ├── SDL_blit_N.c.o.d
│   │   │   │   │   ├── SDL_blit_slow.c.o
│   │   │   │   │   ├── SDL_blit_slow.c.o.d
│   │   │   │   │   ├── SDL_blit.c.o
│   │   │   │   │   ├── SDL_blit.c.o.d
│   │   │   │   │   ├── SDL_bmp.c.o
│   │   │   │   │   ├── SDL_bmp.c.o.d
│   │   │   │   │   ├── SDL_clipboard.c.o
│   │   │   │   │   ├── SDL_clipboard.c.o.d
│   │   │   │   │   ├── SDL_egl.c.o
│   │   │   │   │   ├── SDL_egl.c.o.d
│   │   │   │   │   ├── SDL_fillrect.c.o
│   │   │   │   │   ├── SDL_fillrect.c.o.d
│   │   │   │   │   ├── SDL_pixels.c.o
│   │   │   │   │   ├── SDL_pixels.c.o.d
│   │   │   │   │   ├── SDL_rect.c.o
│   │   │   │   │   ├── SDL_rect.c.o.d
│   │   │   │   │   ├── SDL_RLEaccel.c.o
│   │   │   │   │   ├── SDL_RLEaccel.c.o.d
│   │   │   │   │   ├── SDL_rotate.c.o
│   │   │   │   │   ├── SDL_rotate.c.o.d
│   │   │   │   │   ├── SDL_stb.c.o
│   │   │   │   │   ├── SDL_stb.c.o.d
│   │   │   │   │   ├── SDL_stretch.c.o
│   │   │   │   │   ├── SDL_stretch.c.o.d
│   │   │   │   │   ├── SDL_surface.c.o
│   │   │   │   │   ├── SDL_surface.c.o.d
│   │   │   │   │   ├── SDL_video_unsupported.c.o
│   │   │   │   │   ├── SDL_video_unsupported.c.o.d
│   │   │   │   │   ├── SDL_video.c.o
│   │   │   │   │   ├── SDL_video.c.o.d
│   │   │   │   │   ├── SDL_vulkan_utils.c.o
│   │   │   │   │   ├── SDL_vulkan_utils.c.o.d
│   │   │   │   │   ├── SDL_yuv.c.o
│   │   │   │   │   └── SDL_yuv.c.o.d
│   │   │   │   ├── SDL_assert.c.o
│   │   │   │   ├── SDL_assert.c.o.d
│   │   │   │   ├── SDL_error.c.o
│   │   │   │   ├── SDL_error.c.o.d
│   │   │   │   ├── SDL_guid.c.o
│   │   │   │   ├── SDL_guid.c.o.d
│   │   │   │   ├── SDL_hashtable.c.o
│   │   │   │   ├── SDL_hashtable.c.o.d
│   │   │   │   ├── SDL_hints.c.o
│   │   │   │   ├── SDL_hints.c.o.d
│   │   │   │   ├── SDL_list.c.o
│   │   │   │   ├── SDL_list.c.o.d
│   │   │   │   ├── SDL_log.c.o
│   │   │   │   ├── SDL_log.c.o.d
│   │   │   │   ├── SDL_properties.c.o
│   │   │   │   ├── SDL_properties.c.o.d
│   │   │   │   ├── SDL_utils.c.o
│   │   │   │   ├── SDL_utils.c.o.d
│   │   │   │   ├── SDL.c.o
│   │   │   │   └── SDL.c.o.d
│   │   │   ├── wayland-generated-protocols/
│   │   │   │   ├── alpha-modifier-v1-protocol.c.o
│   │   │   │   ├── alpha-modifier-v1-protocol.c.o.d
│   │   │   │   ├── color-management-v1-protocol.c.o
│   │   │   │   ├── color-management-v1-protocol.c.o.d
│   │   │   │   ├── cursor-shape-v1-protocol.c.o
│   │   │   │   ├── cursor-shape-v1-protocol.c.o.d
│   │   │   │   ├── fractional-scale-v1-protocol.c.o
│   │   │   │   ├── fractional-scale-v1-protocol.c.o.d
│   │   │   │   ├── frog-color-management-v1-protocol.c.o
│   │   │   │   ├── frog-color-management-v1-protocol.c.o.d
│   │   │   │   ├── idle-inhibit-unstable-v1-protocol.c.o
│   │   │   │   ├── idle-inhibit-unstable-v1-protocol.c.o.d
│   │   │   │   ├── input-timestamps-unstable-v1-protocol.c.o
│   │   │   │   ├── input-timestamps-unstable-v1-protocol.c.o.d
│   │   │   │   ├── keyboard-shortcuts-inhibit-unstable-v1-protocol.c.o
│   │   │   │   ├── keyboard-shortcuts-inhibit-unstable-v1-protocol.c.o.d
│   │   │   │   ├── pointer-constraints-unstable-v1-protocol.c.o
│   │   │   │   ├── pointer-constraints-unstable-v1-protocol.c.o.d
│   │   │   │   ├── pointer-gestures-unstable-v1-protocol.c.o
│   │   │   │   ├── pointer-gestures-unstable-v1-protocol.c.o.d
│   │   │   │   ├── pointer-warp-v1-protocol.c.o
│   │   │   │   ├── pointer-warp-v1-protocol.c.o.d
│   │   │   │   ├── primary-selection-unstable-v1-protocol.c.o
│   │   │   │   ├── primary-selection-unstable-v1-protocol.c.o.d
│   │   │   │   ├── relative-pointer-unstable-v1-protocol.c.o
│   │   │   │   ├── relative-pointer-unstable-v1-protocol.c.o.d
│   │   │   │   ├── single-pixel-buffer-v1-protocol.c.o
│   │   │   │   ├── single-pixel-buffer-v1-protocol.c.o.d
│   │   │   │   ├── tablet-v2-protocol.c.o
│   │   │   │   ├── tablet-v2-protocol.c.o.d
│   │   │   │   ├── text-input-unstable-v3-protocol.c.o
│   │   │   │   ├── text-input-unstable-v3-protocol.c.o.d
│   │   │   │   ├── viewporter-protocol.c.o
│   │   │   │   ├── viewporter-protocol.c.o.d
│   │   │   │   ├── wayland-protocol.c.o
│   │   │   │   ├── wayland-protocol.c.o.d
│   │   │   │   ├── xdg-activation-v1-protocol.c.o
│   │   │   │   ├── xdg-activation-v1-protocol.c.o.d
│   │   │   │   ├── xdg-decoration-unstable-v1-protocol.c.o
│   │   │   │   ├── xdg-decoration-unstable-v1-protocol.c.o.d
│   │   │   │   ├── xdg-dialog-v1-protocol.c.o
│   │   │   │   ├── xdg-dialog-v1-protocol.c.o.d
│   │   │   │   ├── xdg-foreign-unstable-v2-protocol.c.o
│   │   │   │   ├── xdg-foreign-unstable-v2-protocol.c.o.d
│   │   │   │   ├── xdg-output-unstable-v1-protocol.c.o
│   │   │   │   ├── xdg-output-unstable-v1-protocol.c.o.d
│   │   │   │   ├── xdg-session-management-v1-protocol.c.o
│   │   │   │   ├── xdg-session-management-v1-protocol.c.o.d
│   │   │   │   ├── xdg-shell-protocol.c.o
│   │   │   │   ├── xdg-shell-protocol.c.o.d
│   │   │   │   ├── xdg-toplevel-icon-v1-protocol.c.o
│   │   │   │   ├── xdg-toplevel-icon-v1-protocol.c.o.d
│   │   │   │   ├── xdg-toplevel-tag-v1-protocol.c.o
│   │   │   │   └── xdg-toplevel-tag-v1-protocol.c.o.d
│   │   │   ├── build.make
│   │   │   ├── cmake_clean.cmake
│   │   │   ├── cmake_pch.h
│   │   │   ├── cmake_pch.h.c
│   │   │   ├── cmake_pch.h.gch
│   │   │   ├── cmake_pch.h.gch.d
│   │   │   ├── compiler_depend.internal
│   │   │   ├── compiler_depend.make
│   │   │   ├── compiler_depend.ts
│   │   │   ├── depend.make
│   │   │   ├── DependInfo.cmake
│   │   │   ├── flags.make
│   │   │   ├── link.txt
│   │   │   └── progress.make
│   │   ├── uninstall.dir/
│   │   │   ├── build.make
│   │   │   ├── cmake_clean.cmake
│   │   │   ├── compiler_depend.make
│   │   │   ├── compiler_depend.ts
│   │   │   ├── DependInfo.cmake
│   │   │   └── progress.make
│   │   ├── cmake.check_cache
│   │   ├── cmake.verify_globs
│   │   ├── CMakeConfigureLog.yaml
│   │   ├── CMakeDirectoryInformation.cmake
│   │   ├── CMakeRuleHashes.txt
│   │   ├── dummy.sym
│   │   ├── Makefile.cmake
│   │   ├── Makefile2
│   │   ├── progress.marks
│   │   ├── SDL_build_config.h.intermediate
│   │   ├── TargetDirectories.txt
│   │   └── VerifyGlobs.cmake
│   ├── include-config-relwithdebinfo/
│   │   └── build_config/
│   │       └── SDL_build_config.h
│   ├── include-revision/
│   │   └── SDL3/
│   │       └── SDL_revision.h
│   ├── test/
│   │   ├── CMakeFiles/
│   │   │   ├── CheckCXX/
│   │   │   │   ├── CMakeFiles/
│   │   │   │   │   ├── 3.28.3/
│   │   │   │   │   │   ├── CompilerIdCXX/
│   │   │   │   │   │   │   ├── tmp/
│   │   │   │   │   │   │   ├── a.out
│   │   │   │   │   │   │   └── CMakeCXXCompilerId.cpp
│   │   │   │   │   │   ├── CMakeCXXCompiler.cmake
│   │   │   │   │   │   ├── CMakeDetermineCompilerABI_CXX.bin
│   │   │   │   │   │   └── CMakeSystem.cmake
│   │   │   │   │   ├── CMakeScratch/
│   │   │   │   │   ├── pkgRedirects/
│   │   │   │   │   ├── cmake.check_cache
│   │   │   │   │   ├── CMakeConfigureLog.yaml
│   │   │   │   │   ├── CMakeDirectoryInformation.cmake
│   │   │   │   │   ├── Makefile.cmake
│   │   │   │   │   ├── Makefile2
│   │   │   │   │   ├── progress.marks
│   │   │   │   │   └── TargetDirectories.txt
│   │   │   │   ├── cmake_install.cmake
│   │   │   │   ├── CMakeCache.txt
│   │   │   │   ├── CMakeLists.txt
│   │   │   │   ├── Makefile
│   │   │   │   └── result.cmake
│   │   │   ├── checkkeys.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── checkkeys.c.o
│   │   │   │   ├── checkkeys.c.o.d
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   └── progress.make
│   │   │   ├── childprocess.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── childprocess.c.o
│   │   │   │   ├── childprocess.c.o.d
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   └── progress.make
│   │   │   ├── copy-sdl-test-resources.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   └── progress.make
│   │   │   ├── generate-gamepad_image_headers.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   └── progress.make
│   │   │   ├── generate-glass_png_header.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   └── progress.make
│   │   │   ├── generate-icon_png_header.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   └── progress.make
│   │   │   ├── loopwave.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   ├── loopwave.c.o
│   │   │   │   ├── loopwave.c.o.d
│   │   │   │   └── progress.make
│   │   │   ├── sdltests_utils.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── progress.make
│   │   │   │   ├── testutils.c.o
│   │   │   │   └── testutils.c.o.d
│   │   │   ├── testasyncio.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   ├── progress.make
│   │   │   │   ├── testasyncio.c.o
│   │   │   │   └── testasyncio.c.o.d
│   │   │   ├── testatomic.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   ├── progress.make
│   │   │   │   ├── testatomic.c.o
│   │   │   │   └── testatomic.c.o.d
│   │   │   ├── testaudio.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   ├── progress.make
│   │   │   │   ├── testaudio.c.o
│   │   │   │   └── testaudio.c.o.d
│   │   │   ├── testaudiohotplug.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   ├── progress.make
│   │   │   │   ├── testaudiohotplug.c.o
│   │   │   │   └── testaudiohotplug.c.o.d
│   │   │   ├── testaudioinfo.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   ├── progress.make
│   │   │   │   ├── testaudioinfo.c.o
│   │   │   │   └── testaudioinfo.c.o.d
│   │   │   ├── testaudiorecording.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   ├── progress.make
│   │   │   │   ├── testaudiorecording.c.o
│   │   │   │   └── testaudiorecording.c.o.d
│   │   │   ├── testaudiostreamdynamicresample.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   ├── progress.make
│   │   │   │   ├── testaudiostreamdynamicresample.c.o
│   │   │   │   └── testaudiostreamdynamicresample.c.o.d
│   │   │   ├── testautomation.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   ├── progress.make
│   │   │   │   ├── testautomation_audio.c.o
│   │   │   │   ├── testautomation_audio.c.o.d
│   │   │   │   ├── testautomation_blit.c.o
│   │   │   │   ├── testautomation_blit.c.o.d
│   │   │   │   ├── testautomation_clipboard.c.o
│   │   │   │   ├── testautomation_clipboard.c.o.d
│   │   │   │   ├── testautomation_events.c.o
│   │   │   │   ├── testautomation_events.c.o.d
│   │   │   │   ├── testautomation_guid.c.o
│   │   │   │   ├── testautomation_guid.c.o.d
│   │   │   │   ├── testautomation_hints.c.o
│   │   │   │   ├── testautomation_hints.c.o.d
│   │   │   │   ├── testautomation_images.c.o
│   │   │   │   ├── testautomation_images.c.o.d
│   │   │   │   ├── testautomation_intrinsics.c.o
│   │   │   │   ├── testautomation_intrinsics.c.o.d
│   │   │   │   ├── testautomation_iostream.c.o
│   │   │   │   ├── testautomation_iostream.c.o.d
│   │   │   │   ├── testautomation_joystick.c.o
│   │   │   │   ├── testautomation_joystick.c.o.d
│   │   │   │   ├── testautomation_keyboard.c.o
│   │   │   │   ├── testautomation_keyboard.c.o.d
│   │   │   │   ├── testautomation_log.c.o
│   │   │   │   ├── testautomation_log.c.o.d
│   │   │   │   ├── testautomation_main.c.o
│   │   │   │   ├── testautomation_main.c.o.d
│   │   │   │   ├── testautomation_math.c.o
│   │   │   │   ├── testautomation_math.c.o.d
│   │   │   │   ├── testautomation_mouse.c.o
│   │   │   │   ├── testautomation_mouse.c.o.d
│   │   │   │   ├── testautomation_pixels.c.o
│   │   │   │   ├── testautomation_pixels.c.o.d
│   │   │   │   ├── testautomation_platform.c.o
│   │   │   │   ├── testautomation_platform.c.o.d
│   │   │   │   ├── testautomation_properties.c.o
│   │   │   │   ├── testautomation_properties.c.o.d
│   │   │   │   ├── testautomation_rect.c.o
│   │   │   │   ├── testautomation_rect.c.o.d
│   │   │   │   ├── testautomation_render.c.o
│   │   │   │   ├── testautomation_render.c.o.d
│   │   │   │   ├── testautomation_sdltest.c.o
│   │   │   │   ├── testautomation_sdltest.c.o.d
│   │   │   │   ├── testautomation_stdlib.c.o
│   │   │   │   ├── testautomation_stdlib.c.o.d
│   │   │   │   ├── testautomation_subsystems.c.o
│   │   │   │   ├── testautomation_subsystems.c.o.d
│   │   │   │   ├── testautomation_surface.c.o
│   │   │   │   ├── testautomation_surface.c.o.d
│   │   │   │   ├── testautomation_time.c.o
│   │   │   │   ├── testautomation_time.c.o.d
│   │   │   │   ├── testautomation_timer.c.o
│   │   │   │   ├── testautomation_timer.c.o.d
│   │   │   │   ├── testautomation_video.c.o
│   │   │   │   ├── testautomation_video.c.o.d
│   │   │   │   ├── testautomation.c.o
│   │   │   │   └── testautomation.c.o.d
│   │   │   ├── testbounds.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   ├── progress.make
│   │   │   │   ├── testbounds.c.o
│   │   │   │   └── testbounds.c.o.d
│   │   │   ├── testcamera.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   ├── progress.make
│   │   │   │   ├── testcamera.c.o
│   │   │   │   └── testcamera.c.o.d
│   │   │   ├── testclipboard.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   ├── progress.make
│   │   │   │   ├── testclipboard.c.o
│   │   │   │   └── testclipboard.c.o.d
│   │   │   ├── testcolorspace.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   ├── progress.make
│   │   │   │   ├── testcolorspace.c.o
│   │   │   │   └── testcolorspace.c.o.d
│   │   │   ├── testcontroller.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── gamepadutils.c.o
│   │   │   │   ├── gamepadutils.c.o.d
│   │   │   │   ├── link.txt
│   │   │   │   ├── progress.make
│   │   │   │   ├── testcontroller.c.o
│   │   │   │   └── testcontroller.c.o.d
│   │   │   ├── testcustomcursor.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   ├── progress.make
│   │   │   │   ├── testcustomcursor.c.o
│   │   │   │   └── testcustomcursor.c.o.d
│   │   │   ├── testdescriptor.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   ├── progress.make
│   │   │   │   ├── testdescriptor.c.o
│   │   │   │   └── testdescriptor.c.o.d
│   │   │   ├── testdialog.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   ├── progress.make
│   │   │   │   ├── testdialog.c.o
│   │   │   │   └── testdialog.c.o.d
│   │   │   ├── testdisplayinfo.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   ├── progress.make
│   │   │   │   ├── testdisplayinfo.c.o
│   │   │   │   └── testdisplayinfo.c.o.d
│   │   │   ├── testdlopennote.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   ├── progress.make
│   │   │   │   ├── testdlopennote.c.o
│   │   │   │   └── testdlopennote.c.o.d
│   │   │   ├── testdraw.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   ├── progress.make
│   │   │   │   ├── testdraw.c.o
│   │   │   │   └── testdraw.c.o.d
│   │   │   ├── testdrawchessboard.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   ├── progress.make
│   │   │   │   ├── testdrawchessboard.c.o
│   │   │   │   └── testdrawchessboard.c.o.d
│   │   │   ├── testdropfile.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   ├── progress.make
│   │   │   │   ├── testdropfile.c.o
│   │   │   │   └── testdropfile.c.o.d
│   │   │   ├── testdynaudioreopen.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   ├── progress.make
│   │   │   │   ├── testdynaudioreopen.c.o
│   │   │   │   └── testdynaudioreopen.c.o.d
│   │   │   ├── testerror.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   ├── progress.make
│   │   │   │   ├── testerror.c.o
│   │   │   │   └── testerror.c.o.d
│   │   │   ├── testevdev.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   ├── progress.make
│   │   │   │   ├── testevdev.c.o
│   │   │   │   └── testevdev.c.o.d
│   │   │   ├── testfile.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   ├── progress.make
│   │   │   │   ├── testfile.c.o
│   │   │   │   └── testfile.c.o.d
│   │   │   ├── testfilesystem.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   ├── progress.make
│   │   │   │   ├── testfilesystem.c.o
│   │   │   │   └── testfilesystem.c.o.d
│   │   │   ├── testgeometry.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   ├── progress.make
│   │   │   │   ├── testgeometry.c.o
│   │   │   │   └── testgeometry.c.o.d
│   │   │   ├── testgl.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   ├── progress.make
│   │   │   │   ├── testgl.c.o
│   │   │   │   └── testgl.c.o.d
│   │   │   ├── testgles.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   ├── progress.make
│   │   │   │   ├── testgles.c.o
│   │   │   │   └── testgles.c.o.d
│   │   │   ├── testgles2.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   ├── progress.make
│   │   │   │   ├── testgles2.c.o
│   │   │   │   └── testgles2.c.o.d
│   │   │   ├── testgpu_simple_clear.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   ├── progress.make
│   │   │   │   ├── testgpu_simple_clear.c.o
│   │   │   │   └── testgpu_simple_clear.c.o.d
│   │   │   ├── testgpu_spinning_cube_xr.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   ├── progress.make
│   │   │   │   ├── testgpu_spinning_cube_xr.c.o
│   │   │   │   └── testgpu_spinning_cube_xr.c.o.d
│   │   │   ├── testgpu_spinning_cube.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   ├── progress.make
│   │   │   │   ├── testgpu_spinning_cube.c.o
│   │   │   │   └── testgpu_spinning_cube.c.o.d
│   │   │   ├── testgpurender_effects.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   ├── progress.make
│   │   │   │   ├── testgpurender_effects.c.o
│   │   │   │   └── testgpurender_effects.c.o.d
│   │   │   ├── testgpurender_msdf.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   ├── progress.make
│   │   │   │   ├── testgpurender_msdf.c.o
│   │   │   │   └── testgpurender_msdf.c.o.d
│   │   │   ├── testhaptic.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   ├── progress.make
│   │   │   │   ├── testhaptic.c.o
│   │   │   │   └── testhaptic.c.o.d
│   │   │   ├── testhittesting.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   ├── progress.make
│   │   │   │   ├── testhittesting.c.o
│   │   │   │   └── testhittesting.c.o.d
│   │   │   ├── testhotplug.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   ├── progress.make
│   │   │   │   ├── testhotplug.c.o
│   │   │   │   └── testhotplug.c.o.d
│   │   │   ├── testiconv.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   ├── progress.make
│   │   │   │   ├── testiconv.c.o
│   │   │   │   └── testiconv.c.o.d
│   │   │   ├── testime.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   ├── progress.make
│   │   │   │   ├── testime.c.o
│   │   │   │   └── testime.c.o.d
│   │   │   ├── testintersections.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   ├── progress.make
│   │   │   │   ├── testintersections.c.o
│   │   │   │   └── testintersections.c.o.d
│   │   │   ├── testkeys.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   ├── progress.make
│   │   │   │   ├── testkeys.c.o
│   │   │   │   └── testkeys.c.o.d
│   │   │   ├── testloadso.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   ├── progress.make
│   │   │   │   ├── testloadso.c.o
│   │   │   │   └── testloadso.c.o.d
│   │   │   ├── testlocale.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   ├── progress.make
│   │   │   │   ├── testlocale.c.o
│   │   │   │   └── testlocale.c.o.d
│   │   │   ├── testlock.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   ├── progress.make
│   │   │   │   ├── testlock.c.o
│   │   │   │   └── testlock.c.o.d
│   │   │   ├── testmanymouse.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   ├── progress.make
│   │   │   │   ├── testmanymouse.c.o
│   │   │   │   └── testmanymouse.c.o.d
│   │   │   ├── testmessage.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   ├── progress.make
│   │   │   │   ├── testmessage.c.o
│   │   │   │   └── testmessage.c.o.d
│   │   │   ├── testmodal.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   ├── progress.make
│   │   │   │   ├── testmodal.c.o
│   │   │   │   └── testmodal.c.o.d
│   │   │   ├── testmouse.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   ├── progress.make
│   │   │   │   ├── testmouse.c.o
│   │   │   │   └── testmouse.c.o.d
│   │   │   ├── testmultiaudio.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   ├── progress.make
│   │   │   │   ├── testmultiaudio.c.o
│   │   │   │   └── testmultiaudio.c.o.d
│   │   │   ├── testnative.dir/
│   │   │   │   ├── __/
│   │   │   │   │   └── wayland-generated-protocols/
│   │   │   │   │       ├── xdg-shell-protocol.c.o
│   │   │   │   │       └── xdg-shell-protocol.c.o.d
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   ├── progress.make
│   │   │   │   ├── testnative.c.o
│   │   │   │   ├── testnative.c.o.d
│   │   │   │   ├── testnativewayland.c.o
│   │   │   │   ├── testnativewayland.c.o.d
│   │   │   │   ├── testnativex11.c.o
│   │   │   │   └── testnativex11.c.o.d
│   │   │   ├── testnotification.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   ├── progress.make
│   │   │   │   ├── testnotification.c.o
│   │   │   │   └── testnotification.c.o.d
│   │   │   ├── testoffscreen.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   ├── progress.make
│   │   │   │   ├── testoffscreen.c.o
│   │   │   │   └── testoffscreen.c.o.d
│   │   │   ├── testoverlay.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   ├── progress.make
│   │   │   │   ├── testoverlay.c.o
│   │   │   │   └── testoverlay.c.o.d
│   │   │   ├── testpalette.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   ├── progress.make
│   │   │   │   ├── testpalette.c.o
│   │   │   │   └── testpalette.c.o.d
│   │   │   ├── testpen.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   ├── progress.make
│   │   │   │   ├── testpen.c.o
│   │   │   │   └── testpen.c.o.d
│   │   │   ├── testplatform.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   ├── progress.make
│   │   │   │   ├── testplatform.c.o
│   │   │   │   └── testplatform.c.o.d
│   │   │   ├── testpopup.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   ├── progress.make
│   │   │   │   ├── testpopup.c.o
│   │   │   │   └── testpopup.c.o.d
│   │   │   ├── testpower.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   ├── progress.make
│   │   │   │   ├── testpower.c.o
│   │   │   │   └── testpower.c.o.d
│   │   │   ├── testprocess.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   ├── progress.make
│   │   │   │   ├── testprocess.c.o
│   │   │   │   └── testprocess.c.o.d
│   │   │   ├── testqsort.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   ├── progress.make
│   │   │   │   ├── testqsort.c.o
│   │   │   │   └── testqsort.c.o.d
│   │   │   ├── testrelative.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   ├── progress.make
│   │   │   │   ├── testrelative.c.o
│   │   │   │   └── testrelative.c.o.d
│   │   │   ├── testrendercopyex.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   ├── progress.make
│   │   │   │   ├── testrendercopyex.c.o
│   │   │   │   └── testrendercopyex.c.o.d
│   │   │   ├── testrendertarget.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   ├── progress.make
│   │   │   │   ├── testrendertarget.c.o
│   │   │   │   └── testrendertarget.c.o.d
│   │   │   ├── testresample.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   ├── progress.make
│   │   │   │   ├── testresample.c.o
│   │   │   │   └── testresample.c.o.d
│   │   │   ├── testrotate.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   ├── progress.make
│   │   │   │   ├── testrotate.c.o
│   │   │   │   └── testrotate.c.o.d
│   │   │   ├── testrumble.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   ├── progress.make
│   │   │   │   ├── testrumble.c.o
│   │   │   │   └── testrumble.c.o.d
│   │   │   ├── testrwlock.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   ├── progress.make
│   │   │   │   ├── testrwlock.c.o
│   │   │   │   └── testrwlock.c.o.d
│   │   │   ├── testscale.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   ├── progress.make
│   │   │   │   ├── testscale.c.o
│   │   │   │   └── testscale.c.o.d
│   │   │   ├── testsem.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   ├── progress.make
│   │   │   │   ├── testsem.c.o
│   │   │   │   └── testsem.c.o.d
│   │   │   ├── testsensor.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   ├── progress.make
│   │   │   │   ├── testsensor.c.o
│   │   │   │   └── testsensor.c.o.d
│   │   │   ├── testshader.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   ├── progress.make
│   │   │   │   ├── testshader.c.o
│   │   │   │   └── testshader.c.o.d
│   │   │   ├── testshape.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   ├── progress.make
│   │   │   │   ├── testshape.c.o
│   │   │   │   └── testshape.c.o.d
│   │   │   ├── testsoftwaretransparent.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   ├── progress.make
│   │   │   │   ├── testsoftwaretransparent.c.o
│   │   │   │   └── testsoftwaretransparent.c.o.d
│   │   │   ├── testsprite.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   ├── progress.make
│   │   │   │   ├── testsprite.c.o
│   │   │   │   └── testsprite.c.o.d
│   │   │   ├── testspritecxx.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   ├── progress.make
│   │   │   │   ├── testsprite.cpp.o
│   │   │   │   └── testsprite.cpp.o.d
│   │   │   ├── testspriteminimal.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   ├── progress.make
│   │   │   │   ├── testspriteminimal.c.o
│   │   │   │   └── testspriteminimal.c.o.d
│   │   │   ├── testspritesurface.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   ├── progress.make
│   │   │   │   ├── testspritesurface.c.o
│   │   │   │   └── testspritesurface.c.o.d
│   │   │   ├── testsurround.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   ├── progress.make
│   │   │   │   ├── testsurround.c.o
│   │   │   │   └── testsurround.c.o.d
│   │   │   ├── testsymbols.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   ├── progress.make
│   │   │   │   ├── testsymbols.c.o
│   │   │   │   └── testsymbols.c.o.d
│   │   │   ├── testthread.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   ├── progress.make
│   │   │   │   ├── testthread.c.o
│   │   │   │   └── testthread.c.o.d
│   │   │   ├── testtime.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   ├── progress.make
│   │   │   │   ├── testtime.c.o
│   │   │   │   └── testtime.c.o.d
│   │   │   ├── testtimer.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   ├── progress.make
│   │   │   │   ├── testtimer.c.o
│   │   │   │   └── testtimer.c.o.d
│   │   │   ├── testtray.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   ├── progress.make
│   │   │   │   ├── testtray.c.o
│   │   │   │   └── testtray.c.o.d
│   │   │   ├── testurl.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   ├── progress.make
│   │   │   │   ├── testurl.c.o
│   │   │   │   └── testurl.c.o.d
│   │   │   ├── testver.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   ├── progress.make
│   │   │   │   ├── testver.c.o
│   │   │   │   └── testver.c.o.d
│   │   │   ├── testviewport.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   ├── progress.make
│   │   │   │   ├── testviewport.c.o
│   │   │   │   └── testviewport.c.o.d
│   │   │   ├── testvulkan.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   ├── progress.make
│   │   │   │   ├── testvulkan.c.o
│   │   │   │   └── testvulkan.c.o.d
│   │   │   ├── testwaylandcustom.dir/
│   │   │   │   ├── __/
│   │   │   │   │   └── wayland-generated-protocols/
│   │   │   │   │       ├── xdg-shell-protocol.c.o
│   │   │   │   │       └── xdg-shell-protocol.c.o.d
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   ├── progress.make
│   │   │   │   ├── testwaylandcustom.c.o
│   │   │   │   └── testwaylandcustom.c.o.d
│   │   │   ├── testwm.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   ├── progress.make
│   │   │   │   ├── testwm.c.o
│   │   │   │   └── testwm.c.o.d
│   │   │   ├── testx11windowdestroy.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.internal
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   ├── progress.make
│   │   │   │   ├── testx11windowdestroy.c.o
│   │   │   │   └── testx11windowdestroy.c.o.d
│   │   │   ├── testyuv.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   ├── progress.make
│   │   │   │   ├── testyuv_cvt.c.o
│   │   │   │   ├── testyuv_cvt.c.o.d
│   │   │   │   ├── testyuv.c.o
│   │   │   │   └── testyuv.c.o.d
│   │   │   ├── torturethread.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   ├── progress.make
│   │   │   │   ├── torturethread.c.o
│   │   │   │   └── torturethread.c.o.d
│   │   │   ├── CMakeDirectoryInformation.cmake
│   │   │   └── progress.marks
│   │   ├── audiofile.png
│   │   ├── checkkeys
│   │   ├── child1-stdin.txt
│   │   ├── child2-stdin.txt
│   │   ├── childprocess
│   │   ├── cmake_install.cmake
│   │   ├── CTestTestfile.cmake
│   │   ├── gamepad_axis_arrow.h
│   │   ├── gamepad_axis_arrow.png
│   │   ├── gamepad_axis.h
│   │   ├── gamepad_axis.png
│   │   ├── gamepad_back.h
│   │   ├── gamepad_back.png
│   │   ├── gamepad_battery_unknown.png
│   │   ├── gamepad_battery_wired.h
│   │   ├── gamepad_battery_wired.png
│   │   ├── gamepad_battery.h
│   │   ├── gamepad_battery.png
│   │   ├── gamepad_button_background.h
│   │   ├── gamepad_button_background.png
│   │   ├── gamepad_button_small.h
│   │   ├── gamepad_button_small.png
│   │   ├── gamepad_button.h
│   │   ├── gamepad_button.png
│   │   ├── gamepad_dual_touchpad.h
│   │   ├── gamepad_dual_touchpad.png
│   │   ├── gamepad_face_abxy.h
│   │   ├── gamepad_face_abxy.png
│   │   ├── gamepad_face_axby.h
│   │   ├── gamepad_face_axby.png
│   │   ├── gamepad_face_bayx.h
│   │   ├── gamepad_face_bayx.png
│   │   ├── gamepad_face_sony.h
│   │   ├── gamepad_face_sony.png
│   │   ├── gamepad_front.h
│   │   ├── gamepad_front.png
│   │   ├── gamepad_grip_sense.h
│   │   ├── gamepad_grip_sense.png
│   │   ├── gamepad_touchpad.h
│   │   ├── gamepad_touchpad.png
│   │   ├── gamepad_wired.h
│   │   ├── gamepad_wired.png
│   │   ├── gamepad_wireless.h
│   │   ├── gamepad_wireless.png
│   │   ├── glass.h
│   │   ├── glass.png
│   │   ├── icon.h
│   │   ├── icon.png
│   │   ├── icon2x.png
│   │   ├── logaudiodev.png
│   │   ├── loopwave
│   │   ├── Makefile
│   │   ├── moose.dat
│   │   ├── msdf_font.csv
│   │   ├── msdf_font.png
│   │   ├── physaudiodev.png
│   │   ├── sample.png
│   │   ├── sample.wav
│   │   ├── sdl-test_round.png
│   │   ├── soundboard_levels.png
│   │   ├── soundboard.png
│   │   ├── speaker.png
│   │   ├── sword.wav
│   │   ├── test_redirection_stderr.txt
│   │   ├── test_redirection_stdin.txt
│   │   ├── test_redirection_stdout.txt
│   │   ├── testasyncio
│   │   ├── testatomic
│   │   ├── testaudio
│   │   ├── testaudiohotplug
│   │   ├── testaudioinfo
│   │   ├── testaudiorecording
│   │   ├── testaudiostreamdynamicresample
│   │   ├── testautomation
│   │   ├── testbounds
│   │   ├── testcamera
│   │   ├── testclipboard
│   │   ├── testcolorspace
│   │   ├── testcontroller
│   │   ├── testcustomcursor
│   │   ├── testdescriptor
│   │   ├── testdialog
│   │   ├── testdisplayinfo
│   │   ├── testdlopennote
│   │   ├── testdraw
│   │   ├── testdrawchessboard
│   │   ├── testdropfile
│   │   ├── testdynaudioreopen
│   │   ├── testerror
│   │   ├── testevdev
│   │   ├── testfile
│   │   ├── testfilesystem
│   │   ├── testgeometry
│   │   ├── testgl
│   │   ├── testgles
│   │   ├── testgles2
│   │   ├── testgpu_simple_clear
│   │   ├── testgpu_spinning_cube
│   │   ├── testgpu_spinning_cube_xr
│   │   ├── testgpurender_effects
│   │   ├── testgpurender_msdf
│   │   ├── testhaptic
│   │   ├── testhittesting
│   │   ├── testhotplug
│   │   ├── testiconv
│   │   ├── testime
│   │   ├── testintersections
│   │   ├── testkeys
│   │   ├── testloadso
│   │   ├── testlocale
│   │   ├── testlock
│   │   ├── testmanymouse
│   │   ├── testmessage
│   │   ├── testmodal
│   │   ├── testmouse
│   │   ├── testmultiaudio
│   │   ├── testnative
│   │   ├── testnotification
│   │   ├── testoffscreen
│   │   ├── testoverlay
│   │   ├── testpalette
│   │   ├── testpen
│   │   ├── testplatform
│   │   ├── testpopup
│   │   ├── testpower
│   │   ├── testprocess
│   │   ├── testqsort
│   │   ├── testrelative
│   │   ├── testrendercopyex
│   │   ├── testrendertarget
│   │   ├── testresample
│   │   ├── testrotate
│   │   ├── testrumble
│   │   ├── testrwlock
│   │   ├── testscale
│   │   ├── testsem
│   │   ├── testsensor
│   │   ├── testshader
│   │   ├── testshape
│   │   ├── testsoftwaretransparent
│   │   ├── testsprite
│   │   ├── testspritecxx
│   │   ├── testspriteminimal
│   │   ├── testspritesurface
│   │   ├── testsurround
│   │   ├── testsymbols
│   │   ├── testthread
│   │   ├── testtime
│   │   ├── testtimer
│   │   ├── testtray
│   │   ├── testurl
│   │   ├── testver
│   │   ├── testviewport
│   │   ├── testvulkan
│   │   ├── testwaylandcustom
│   │   ├── testwm
│   │   ├── testx11windowdestroy
│   │   ├── testx11windowdestroy_baseline
│   │   ├── testyuv
│   │   ├── testyuv.png
│   │   ├── torturethread
│   │   ├── trashcan.png
│   │   ├── unifont-15.1.05.hex
│   │   └── utf8.txt
│   ├── Testing/
│   │   └── Temporary/
│   │       ├── CTestCostData.txt
│   │       ├── LastTest.log
│   │       └── LastTestsFailed.log
│   ├── wayland-generated-protocols/
│   │   ├── alpha-modifier-v1-client-protocol.h
│   │   ├── alpha-modifier-v1-protocol.c
│   │   ├── color-management-v1-client-protocol.h
│   │   ├── color-management-v1-protocol.c
│   │   ├── cursor-shape-v1-client-protocol.h
│   │   ├── cursor-shape-v1-protocol.c
│   │   ├── fractional-scale-v1-client-protocol.h
│   │   ├── fractional-scale-v1-protocol.c
│   │   ├── frog-color-management-v1-client-protocol.h
│   │   ├── frog-color-management-v1-protocol.c
│   │   ├── idle-inhibit-unstable-v1-client-protocol.h
│   │   ├── idle-inhibit-unstable-v1-protocol.c
│   │   ├── input-timestamps-unstable-v1-client-protocol.h
│   │   ├── input-timestamps-unstable-v1-protocol.c
│   │   ├── keyboard-shortcuts-inhibit-unstable-v1-client-protocol.h
│   │   ├── keyboard-shortcuts-inhibit-unstable-v1-protocol.c
│   │   ├── pointer-constraints-unstable-v1-client-protocol.h
│   │   ├── pointer-constraints-unstable-v1-protocol.c
│   │   ├── pointer-gestures-unstable-v1-client-protocol.h
│   │   ├── pointer-gestures-unstable-v1-protocol.c
│   │   ├── pointer-warp-v1-client-protocol.h
│   │   ├── pointer-warp-v1-protocol.c
│   │   ├── primary-selection-unstable-v1-client-protocol.h
│   │   ├── primary-selection-unstable-v1-protocol.c
│   │   ├── relative-pointer-unstable-v1-client-protocol.h
│   │   ├── relative-pointer-unstable-v1-protocol.c
│   │   ├── single-pixel-buffer-v1-client-protocol.h
│   │   ├── single-pixel-buffer-v1-protocol.c
│   │   ├── tablet-v2-client-protocol.h
│   │   ├── tablet-v2-protocol.c
│   │   ├── text-input-unstable-v3-client-protocol.h
│   │   ├── text-input-unstable-v3-protocol.c
│   │   ├── viewporter-client-protocol.h
│   │   ├── viewporter-protocol.c
│   │   ├── wayland-client-protocol.h
│   │   ├── wayland-protocol.c
│   │   ├── xdg-activation-v1-client-protocol.h
│   │   ├── xdg-activation-v1-protocol.c
│   │   ├── xdg-decoration-unstable-v1-client-protocol.h
│   │   ├── xdg-decoration-unstable-v1-protocol.c
│   │   ├── xdg-dialog-v1-client-protocol.h
│   │   ├── xdg-dialog-v1-protocol.c
│   │   ├── xdg-foreign-unstable-v2-client-protocol.h
│   │   ├── xdg-foreign-unstable-v2-protocol.c
│   │   ├── xdg-output-unstable-v1-client-protocol.h
│   │   ├── xdg-output-unstable-v1-protocol.c
│   │   ├── xdg-session-management-v1-client-protocol.h
│   │   ├── xdg-session-management-v1-protocol.c
│   │   ├── xdg-shell-client-protocol.h
│   │   ├── xdg-shell-protocol.c
│   │   ├── xdg-toplevel-icon-v1-client-protocol.h
│   │   ├── xdg-toplevel-icon-v1-protocol.c
│   │   ├── xdg-toplevel-tag-v1-client-protocol.h
│   │   └── xdg-toplevel-tag-v1-protocol.c
│   ├── cmake_install.cmake
│   ├── cmake_uninstall.cmake
│   ├── CMakeCache.txt
│   ├── CPackConfig.cmake
│   ├── CPackProjectConfig.cmake
│   ├── CPackSourceConfig.cmake
│   ├── CTestTestfile.cmake
│   ├── libSDL_uclibc.a
│   ├── libSDL3_test.a
│   ├── libSDL3.so.0.5.0
│   ├── Makefile
│   ├── sdl3.pc
│   ├── SDL3Config.cmake
│   ├── SDL3ConfigVersion.cmake
│   ├── SDL3headersTargets.cmake
│   ├── SDL3sharedTargets.cmake
│   ├── SDL3testTargets.cmake
│   ├── x11_destroy_test
│   ├── x11_destroy_test.c
│   ├── x11_destroy_test.id
│   ├── x11_destroy_window
│   ├── x11_destroy_window.c
│   ├── x11_error_chain_test
│   └── x11_error_chain_test.c
├── build-asan/
│   ├── CMakeFiles/
│   │   ├── 3.28.3/
│   │   │   ├── CompilerIdC/
│   │   │   │   ├── tmp/
│   │   │   │   ├── a.out
│   │   │   │   └── CMakeCCompilerId.c
│   │   │   ├── CompilerIdCXX/
│   │   │   │   ├── tmp/
│   │   │   │   ├── a.out
│   │   │   │   └── CMakeCXXCompilerId.cpp
│   │   │   ├── CMakeCCompiler.cmake
│   │   │   ├── CMakeCXXCompiler.cmake
│   │   │   ├── CMakeDetermineCompilerABI_C.bin
│   │   │   ├── CMakeDetermineCompilerABI_CXX.bin
│   │   │   └── CMakeSystem.cmake
│   │   ├── CMakeScratch/
│   │   ├── CMakeTmp/
│   │   │   ├── SDL_detect_arch/
│   │   │   │   ├── CMakeFiles/
│   │   │   │   │   └── CMakeTmp/
│   │   │   │   └── bin
│   │   │   └── SDL_detect_arch.c
│   │   ├── Export/
│   │   │   └── 35815d1d52a6ea1175d74784b559bdb6/
│   │   │       ├── SDL3headersTargets.cmake
│   │   │       ├── SDL3sharedTargets-relwithdebinfo.cmake
│   │   │       ├── SDL3sharedTargets.cmake
│   │   │       ├── SDL3testTargets-relwithdebinfo.cmake
│   │   │       └── SDL3testTargets.cmake
│   │   ├── git-data/
│   │   │   ├── grabRef.cmake
│   │   │   ├── HEAD
│   │   │   └── head-ref
│   │   ├── pkgRedirects/
│   │   ├── SDL_uclibc.dir/
│   │   │   ├── src/
│   │   │   │   └── libm/
│   │   │   │       ├── e_atan2.c.o
│   │   │   │       ├── e_atan2.c.o.d
│   │   │   │       ├── e_exp.c.o
│   │   │   │       ├── e_exp.c.o.d
│   │   │   │       ├── e_fmod.c.o
│   │   │   │       ├── e_fmod.c.o.d
│   │   │   │       ├── e_log.c.o
│   │   │   │       ├── e_log.c.o.d
│   │   │   │       ├── e_log10.c.o
│   │   │   │       ├── e_log10.c.o.d
│   │   │   │       ├── e_pow.c.o
│   │   │   │       ├── e_pow.c.o.d
│   │   │   │       ├── e_rem_pio2.c.o
│   │   │   │       ├── e_rem_pio2.c.o.d
│   │   │   │       ├── e_sqrt.c.o
│   │   │   │       ├── e_sqrt.c.o.d
│   │   │   │       ├── k_cos.c.o
│   │   │   │       ├── k_cos.c.o.d
│   │   │   │       ├── k_rem_pio2.c.o
│   │   │   │       ├── k_rem_pio2.c.o.d
│   │   │   │       ├── k_sin.c.o
│   │   │   │       ├── k_sin.c.o.d
│   │   │   │       ├── k_tan.c.o
│   │   │   │       ├── k_tan.c.o.d
│   │   │   │       ├── s_atan.c.o
│   │   │   │       ├── s_atan.c.o.d
│   │   │   │       ├── s_copysign.c.o
│   │   │   │       ├── s_copysign.c.o.d
│   │   │   │       ├── s_cos.c.o
│   │   │   │       ├── s_cos.c.o.d
│   │   │   │       ├── s_fabs.c.o
│   │   │   │       ├── s_fabs.c.o.d
│   │   │   │       ├── s_floor.c.o
│   │   │   │       ├── s_floor.c.o.d
│   │   │   │       ├── s_isinf.c.o
│   │   │   │       ├── s_isinf.c.o.d
│   │   │   │       ├── s_isinff.c.o
│   │   │   │       ├── s_isinff.c.o.d
│   │   │   │       ├── s_isnan.c.o
│   │   │   │       ├── s_isnan.c.o.d
│   │   │   │       ├── s_isnanf.c.o
│   │   │   │       ├── s_isnanf.c.o.d
│   │   │   │       ├── s_modf.c.o
│   │   │   │       ├── s_modf.c.o.d
│   │   │   │       ├── s_scalbn.c.o
│   │   │   │       ├── s_scalbn.c.o.d
│   │   │   │       ├── s_sin.c.o
│   │   │   │       ├── s_sin.c.o.d
│   │   │   │       ├── s_tan.c.o
│   │   │   │       └── s_tan.c.o.d
│   │   │   ├── build.make
│   │   │   ├── cmake_clean_target.cmake
│   │   │   ├── cmake_clean.cmake
│   │   │   ├── compiler_depend.internal
│   │   │   ├── compiler_depend.make
│   │   │   ├── compiler_depend.ts
│   │   │   ├── depend.make
│   │   │   ├── DependInfo.cmake
│   │   │   ├── flags.make
│   │   │   ├── link.txt
│   │   │   └── progress.make
│   │   ├── SDL3_test.dir/
│   │   │   ├── src/
│   │   │   │   └── test/
│   │   │   │       ├── SDL_test_assert.c.o
│   │   │   │       ├── SDL_test_assert.c.o.d
│   │   │   │       ├── SDL_test_common.c.o
│   │   │   │       ├── SDL_test_common.c.o.d
│   │   │   │       ├── SDL_test_compare.c.o
│   │   │   │       ├── SDL_test_compare.c.o.d
│   │   │   │       ├── SDL_test_crc32.c.o
│   │   │   │       ├── SDL_test_crc32.c.o.d
│   │   │   │       ├── SDL_test_font.c.o
│   │   │   │       ├── SDL_test_font.c.o.d
│   │   │   │       ├── SDL_test_fuzzer.c.o
│   │   │   │       ├── SDL_test_fuzzer.c.o.d
│   │   │   │       ├── SDL_test_harness.c.o
│   │   │   │       ├── SDL_test_harness.c.o.d
│   │   │   │       ├── SDL_test_log.c.o
│   │   │   │       ├── SDL_test_log.c.o.d
│   │   │   │       ├── SDL_test_md5.c.o
│   │   │   │       ├── SDL_test_md5.c.o.d
│   │   │   │       ├── SDL_test_memory.c.o
│   │   │   │       └── SDL_test_memory.c.o.d
│   │   │   ├── build.make
│   │   │   ├── cmake_clean_target.cmake
│   │   │   ├── cmake_clean.cmake
│   │   │   ├── compiler_depend.internal
│   │   │   ├── compiler_depend.make
│   │   │   ├── compiler_depend.ts
│   │   │   ├── depend.make
│   │   │   ├── DependInfo.cmake
│   │   │   ├── flags.make
│   │   │   ├── link.txt
│   │   │   └── progress.make
│   │   ├── SDL3-shared.dir/
│   │   │   ├── src/
│   │   │   │   ├── atomic/
│   │   │   │   │   ├── SDL_atomic.c.o
│   │   │   │   │   ├── SDL_atomic.c.o.d
│   │   │   │   │   ├── SDL_spinlock.c.o
│   │   │   │   │   └── SDL_spinlock.c.o.d
│   │   │   │   ├── audio/
│   │   │   │   │   ├── alsa/
│   │   │   │   │   │   ├── SDL_alsa_audio.c.o
│   │   │   │   │   │   └── SDL_alsa_audio.c.o.d
│   │   │   │   │   ├── disk/
│   │   │   │   │   │   ├── SDL_diskaudio.c.o
│   │   │   │   │   │   └── SDL_diskaudio.c.o.d
│   │   │   │   │   ├── dummy/
│   │   │   │   │   │   ├── SDL_dummyaudio.c.o
│   │   │   │   │   │   └── SDL_dummyaudio.c.o.d
│   │   │   │   │   ├── pipewire/
│   │   │   │   │   │   ├── SDL_pipewire.c.o
│   │   │   │   │   │   └── SDL_pipewire.c.o.d
│   │   │   │   │   ├── pulseaudio/
│   │   │   │   │   │   ├── SDL_pulseaudio.c.o
│   │   │   │   │   │   └── SDL_pulseaudio.c.o.d
│   │   │   │   │   ├── SDL_audio.c.o
│   │   │   │   │   ├── SDL_audio.c.o.d
│   │   │   │   │   ├── SDL_audiocvt.c.o
│   │   │   │   │   ├── SDL_audiocvt.c.o.d
│   │   │   │   │   ├── SDL_audiodev.c.o
│   │   │   │   │   ├── SDL_audiodev.c.o.d
│   │   │   │   │   ├── SDL_audioqueue.c.o
│   │   │   │   │   ├── SDL_audioqueue.c.o.d
│   │   │   │   │   ├── SDL_audioresample.c.o
│   │   │   │   │   ├── SDL_audioresample.c.o.d
│   │   │   │   │   ├── SDL_audiotypecvt.c.o
│   │   │   │   │   ├── SDL_audiotypecvt.c.o.d
│   │   │   │   │   ├── SDL_mixer.c.o
│   │   │   │   │   ├── SDL_mixer.c.o.d
│   │   │   │   │   ├── SDL_wave.c.o
│   │   │   │   │   └── SDL_wave.c.o.d
│   │   │   │   ├── camera/
│   │   │   │   │   ├── dummy/
│   │   │   │   │   │   ├── SDL_camera_dummy.c.o
│   │   │   │   │   │   └── SDL_camera_dummy.c.o.d
│   │   │   │   │   ├── pipewire/
│   │   │   │   │   │   ├── SDL_camera_pipewire.c.o
│   │   │   │   │   │   └── SDL_camera_pipewire.c.o.d
│   │   │   │   │   ├── v4l2/
│   │   │   │   │   │   ├── SDL_camera_v4l2.c.o
│   │   │   │   │   │   └── SDL_camera_v4l2.c.o.d
│   │   │   │   │   ├── SDL_camera.c.o
│   │   │   │   │   └── SDL_camera.c.o.d
│   │   │   │   ├── core/
│   │   │   │   │   ├── linux/
│   │   │   │   │   │   ├── SDL_dbus.c.o
│   │   │   │   │   │   ├── SDL_dbus.c.o.d
│   │   │   │   │   │   ├── SDL_evdev_capabilities.c.o
│   │   │   │   │   │   ├── SDL_evdev_capabilities.c.o.d
│   │   │   │   │   │   ├── SDL_evdev_kbd.c.o
│   │   │   │   │   │   ├── SDL_evdev_kbd.c.o.d
│   │   │   │   │   │   ├── SDL_evdev.c.o
│   │   │   │   │   │   ├── SDL_evdev.c.o.d
│   │   │   │   │   │   ├── SDL_fcitx.c.o
│   │   │   │   │   │   ├── SDL_fcitx.c.o.d
│   │   │   │   │   │   ├── SDL_ibus.c.o
│   │   │   │   │   │   ├── SDL_ibus.c.o.d
│   │   │   │   │   │   ├── SDL_ime.c.o
│   │   │   │   │   │   ├── SDL_ime.c.o.d
│   │   │   │   │   │   ├── SDL_progressbar.c.o
│   │   │   │   │   │   ├── SDL_progressbar.c.o.d
│   │   │   │   │   │   ├── SDL_system_theme.c.o
│   │   │   │   │   │   ├── SDL_system_theme.c.o.d
│   │   │   │   │   │   ├── SDL_threadprio.c.o
│   │   │   │   │   │   ├── SDL_threadprio.c.o.d
│   │   │   │   │   │   ├── SDL_ubuntu_touch.c.o
│   │   │   │   │   │   ├── SDL_ubuntu_touch.c.o.d
│   │   │   │   │   │   ├── SDL_udev.c.o
│   │   │   │   │   │   └── SDL_udev.c.o.d
│   │   │   │   │   ├── unix/
│   │   │   │   │   │   ├── SDL_appid.c.o
│   │   │   │   │   │   ├── SDL_appid.c.o.d
│   │   │   │   │   │   ├── SDL_fribidi.c.o
│   │   │   │   │   │   ├── SDL_fribidi.c.o.d
│   │   │   │   │   │   ├── SDL_libthai.c.o
│   │   │   │   │   │   ├── SDL_libthai.c.o.d
│   │   │   │   │   │   ├── SDL_poll.c.o
│   │   │   │   │   │   └── SDL_poll.c.o.d
│   │   │   │   │   ├── SDL_core_unsupported.c.o
│   │   │   │   │   └── SDL_core_unsupported.c.o.d
│   │   │   │   ├── cpuinfo/
│   │   │   │   │   ├── SDL_cpuinfo.c.o
│   │   │   │   │   └── SDL_cpuinfo.c.o.d
│   │   │   │   ├── dialog/
│   │   │   │   │   ├── unix/
│   │   │   │   │   │   ├── SDL_portaldialog.c.o
│   │   │   │   │   │   ├── SDL_portaldialog.c.o.d
│   │   │   │   │   │   ├── SDL_unixdialog.c.o
│   │   │   │   │   │   ├── SDL_unixdialog.c.o.d
│   │   │   │   │   │   ├── SDL_zenitydialog.c.o
│   │   │   │   │   │   ├── SDL_zenitydialog.c.o.d
│   │   │   │   │   │   ├── SDL_zenitymessagebox.c.o
│   │   │   │   │   │   └── SDL_zenitymessagebox.c.o.d
│   │   │   │   │   ├── SDL_dialog_utils.c.o
│   │   │   │   │   ├── SDL_dialog_utils.c.o.d
│   │   │   │   │   ├── SDL_dialog.c.o
│   │   │   │   │   └── SDL_dialog.c.o.d
│   │   │   │   ├── dynapi/
│   │   │   │   │   ├── SDL_dynapi.c.o
│   │   │   │   │   └── SDL_dynapi.c.o.d
│   │   │   │   ├── events/
│   │   │   │   │   ├── imKStoUCS.c.o
│   │   │   │   │   ├── imKStoUCS.c.o.d
│   │   │   │   │   ├── SDL_categories.c.o
│   │   │   │   │   ├── SDL_categories.c.o.d
│   │   │   │   │   ├── SDL_clipboardevents.c.o
│   │   │   │   │   ├── SDL_clipboardevents.c.o.d
│   │   │   │   │   ├── SDL_displayevents.c.o
│   │   │   │   │   ├── SDL_displayevents.c.o.d
│   │   │   │   │   ├── SDL_dropevents.c.o
│   │   │   │   │   ├── SDL_dropevents.c.o.d
│   │   │   │   │   ├── SDL_events.c.o
│   │   │   │   │   ├── SDL_events.c.o.d
│   │   │   │   │   ├── SDL_eventwatch.c.o
│   │   │   │   │   ├── SDL_eventwatch.c.o.d
│   │   │   │   │   ├── SDL_keyboard.c.o
│   │   │   │   │   ├── SDL_keyboard.c.o.d
│   │   │   │   │   ├── SDL_keymap.c.o
│   │   │   │   │   ├── SDL_keymap.c.o.d
│   │   │   │   │   ├── SDL_keysym_to_keycode.c.o
│   │   │   │   │   ├── SDL_keysym_to_keycode.c.o.d
│   │   │   │   │   ├── SDL_keysym_to_scancode.c.o
│   │   │   │   │   ├── SDL_keysym_to_scancode.c.o.d
│   │   │   │   │   ├── SDL_mouse.c.o
│   │   │   │   │   ├── SDL_mouse.c.o.d
│   │   │   │   │   ├── SDL_notificationevents.c.o
│   │   │   │   │   ├── SDL_notificationevents.c.o.d
│   │   │   │   │   ├── SDL_pen.c.o
│   │   │   │   │   ├── SDL_pen.c.o.d
│   │   │   │   │   ├── SDL_quit.c.o
│   │   │   │   │   ├── SDL_quit.c.o.d
│   │   │   │   │   ├── SDL_scancode_tables.c.o
│   │   │   │   │   ├── SDL_scancode_tables.c.o.d
│   │   │   │   │   ├── SDL_touch.c.o
│   │   │   │   │   ├── SDL_touch.c.o.d
│   │   │   │   │   ├── SDL_windowevents.c.o
│   │   │   │   │   └── SDL_windowevents.c.o.d
│   │   │   │   ├── filesystem/
│   │   │   │   │   ├── posix/
│   │   │   │   │   │   ├── SDL_sysfsops.c.o
│   │   │   │   │   │   └── SDL_sysfsops.c.o.d
│   │   │   │   │   ├── unix/
│   │   │   │   │   │   ├── SDL_sysfilesystem.c.o
│   │   │   │   │   │   └── SDL_sysfilesystem.c.o.d
│   │   │   │   │   ├── SDL_filesystem.c.o
│   │   │   │   │   └── SDL_filesystem.c.o.d
│   │   │   │   ├── gpu/
│   │   │   │   │   ├── vulkan/
│   │   │   │   │   │   ├── SDL_gpu_vulkan.c.o
│   │   │   │   │   │   └── SDL_gpu_vulkan.c.o.d
│   │   │   │   │   ├── xr/
│   │   │   │   │   │   ├── SDL_gpu_openxr.c.o
│   │   │   │   │   │   ├── SDL_gpu_openxr.c.o.d
│   │   │   │   │   │   ├── SDL_openxrdyn.c.o
│   │   │   │   │   │   └── SDL_openxrdyn.c.o.d
│   │   │   │   │   ├── SDL_gpu.c.o
│   │   │   │   │   └── SDL_gpu.c.o.d
│   │   │   │   ├── haptic/
│   │   │   │   │   ├── hidapi/
│   │   │   │   │   │   ├── SDL_hidapihaptic_lg4ff.c.o
│   │   │   │   │   │   ├── SDL_hidapihaptic_lg4ff.c.o.d
│   │   │   │   │   │   ├── SDL_hidapihaptic.c.o
│   │   │   │   │   │   └── SDL_hidapihaptic.c.o.d
│   │   │   │   │   ├── linux/
│   │   │   │   │   │   ├── SDL_syshaptic.c.o
│   │   │   │   │   │   └── SDL_syshaptic.c.o.d
│   │   │   │   │   ├── SDL_haptic.c.o
│   │   │   │   │   └── SDL_haptic.c.o.d
│   │   │   │   ├── hidapi/
│   │   │   │   │   ├── SDL_hidapi.c.o
│   │   │   │   │   └── SDL_hidapi.c.o.d
│   │   │   │   ├── io/
│   │   │   │   │   ├── generic/
│   │   │   │   │   │   ├── SDL_asyncio_generic.c.o
│   │   │   │   │   │   └── SDL_asyncio_generic.c.o.d
│   │   │   │   │   ├── io_uring/
│   │   │   │   │   │   ├── SDL_asyncio_liburing.c.o
│   │   │   │   │   │   └── SDL_asyncio_liburing.c.o.d
│   │   │   │   │   ├── SDL_asyncio.c.o
│   │   │   │   │   ├── SDL_asyncio.c.o.d
│   │   │   │   │   ├── SDL_iostream.c.o
│   │   │   │   │   └── SDL_iostream.c.o.d
│   │   │   │   ├── joystick/
│   │   │   │   │   ├── hidapi/
│   │   │   │   │   │   ├── SDL_hidapi_8bitdo.c.o
│   │   │   │   │   │   ├── SDL_hidapi_8bitdo.c.o.d
│   │   │   │   │   │   ├── SDL_hidapi_combined.c.o
│   │   │   │   │   │   ├── SDL_hidapi_combined.c.o.d
│   │   │   │   │   │   ├── SDL_hidapi_flydigi.c.o
│   │   │   │   │   │   ├── SDL_hidapi_flydigi.c.o.d
│   │   │   │   │   │   ├── SDL_hidapi_gamecube.c.o
│   │   │   │   │   │   ├── SDL_hidapi_gamecube.c.o.d
│   │   │   │   │   │   ├── SDL_hidapi_gamesir.c.o
│   │   │   │   │   │   ├── SDL_hidapi_gamesir.c.o.d
│   │   │   │   │   │   ├── SDL_hidapi_gip.c.o
│   │   │   │   │   │   ├── SDL_hidapi_gip.c.o.d
│   │   │   │   │   │   ├── SDL_hidapi_lg4ff.c.o
│   │   │   │   │   │   ├── SDL_hidapi_lg4ff.c.o.d
│   │   │   │   │   │   ├── SDL_hidapi_luna.c.o
│   │   │   │   │   │   ├── SDL_hidapi_luna.c.o.d
│   │   │   │   │   │   ├── SDL_hidapi_ps3.c.o
│   │   │   │   │   │   ├── SDL_hidapi_ps3.c.o.d
│   │   │   │   │   │   ├── SDL_hidapi_ps4.c.o
│   │   │   │   │   │   ├── SDL_hidapi_ps4.c.o.d
│   │   │   │   │   │   ├── SDL_hidapi_ps5.c.o
│   │   │   │   │   │   ├── SDL_hidapi_ps5.c.o.d
│   │   │   │   │   │   ├── SDL_hidapi_rumble.c.o
│   │   │   │   │   │   ├── SDL_hidapi_rumble.c.o.d
│   │   │   │   │   │   ├── SDL_hidapi_shield.c.o
│   │   │   │   │   │   ├── SDL_hidapi_shield.c.o.d
│   │   │   │   │   │   ├── SDL_hidapi_sinput.c.o
│   │   │   │   │   │   ├── SDL_hidapi_sinput.c.o.d
│   │   │   │   │   │   ├── SDL_hidapi_stadia.c.o
│   │   │   │   │   │   ├── SDL_hidapi_stadia.c.o.d
│   │   │   │   │   │   ├── SDL_hidapi_steam_hori.c.o
│   │   │   │   │   │   ├── SDL_hidapi_steam_hori.c.o.d
│   │   │   │   │   │   ├── SDL_hidapi_steam_triton.c.o
│   │   │   │   │   │   ├── SDL_hidapi_steam_triton.c.o.d
│   │   │   │   │   │   ├── SDL_hidapi_steam.c.o
│   │   │   │   │   │   ├── SDL_hidapi_steam.c.o.d
│   │   │   │   │   │   ├── SDL_hidapi_steamdeck.c.o
│   │   │   │   │   │   ├── SDL_hidapi_steamdeck.c.o.d
│   │   │   │   │   │   ├── SDL_hidapi_switch.c.o
│   │   │   │   │   │   ├── SDL_hidapi_switch.c.o.d
│   │   │   │   │   │   ├── SDL_hidapi_switch2.c.o
│   │   │   │   │   │   ├── SDL_hidapi_switch2.c.o.d
│   │   │   │   │   │   ├── SDL_hidapi_wii.c.o
│   │   │   │   │   │   ├── SDL_hidapi_wii.c.o.d
│   │   │   │   │   │   ├── SDL_hidapi_xbox360.c.o
│   │   │   │   │   │   ├── SDL_hidapi_xbox360.c.o.d
│   │   │   │   │   │   ├── SDL_hidapi_xbox360bb.c.o
│   │   │   │   │   │   ├── SDL_hidapi_xbox360bb.c.o.d
│   │   │   │   │   │   ├── SDL_hidapi_xbox360w.c.o
│   │   │   │   │   │   ├── SDL_hidapi_xbox360w.c.o.d
│   │   │   │   │   │   ├── SDL_hidapi_xboxone.c.o
│   │   │   │   │   │   ├── SDL_hidapi_xboxone.c.o.d
│   │   │   │   │   │   ├── SDL_hidapi_zuiki.c.o
│   │   │   │   │   │   ├── SDL_hidapi_zuiki.c.o.d
│   │   │   │   │   │   ├── SDL_hidapijoystick.c.o
│   │   │   │   │   │   ├── SDL_hidapijoystick.c.o.d
│   │   │   │   │   │   ├── SDL_report_descriptor.c.o
│   │   │   │   │   │   └── SDL_report_descriptor.c.o.d
│   │   │   │   │   ├── linux/
│   │   │   │   │   │   ├── SDL_sysjoystick.c.o
│   │   │   │   │   │   └── SDL_sysjoystick.c.o.d
│   │   │   │   │   ├── virtual/
│   │   │   │   │   │   ├── SDL_virtualjoystick.c.o
│   │   │   │   │   │   └── SDL_virtualjoystick.c.o.d
│   │   │   │   │   ├── controller_type.c.o
│   │   │   │   │   ├── controller_type.c.o.d
│   │   │   │   │   ├── SDL_gamepad.c.o
│   │   │   │   │   ├── SDL_gamepad.c.o.d
│   │   │   │   │   ├── SDL_joystick.c.o
│   │   │   │   │   ├── SDL_joystick.c.o.d
│   │   │   │   │   ├── SDL_steam_virtual_gamepad.c.o
│   │   │   │   │   └── SDL_steam_virtual_gamepad.c.o.d
│   │   │   │   ├── loadso/
│   │   │   │   │   └── dlopen/
│   │   │   │   │       ├── SDL_sysloadso.c.o
│   │   │   │   │       └── SDL_sysloadso.c.o.d
│   │   │   │   ├── locale/
│   │   │   │   │   ├── unix/
│   │   │   │   │   │   ├── SDL_syslocale.c.o
│   │   │   │   │   │   └── SDL_syslocale.c.o.d
│   │   │   │   │   ├── SDL_locale.c.o
│   │   │   │   │   └── SDL_locale.c.o.d
│   │   │   │   ├── main/
│   │   │   │   │   ├── generic/
│   │   │   │   │   │   ├── SDL_sysmain_callbacks.c.o
│   │   │   │   │   │   └── SDL_sysmain_callbacks.c.o.d
│   │   │   │   │   ├── SDL_main_callbacks.c.o
│   │   │   │   │   ├── SDL_main_callbacks.c.o.d
│   │   │   │   │   ├── SDL_runapp.c.o
│   │   │   │   │   └── SDL_runapp.c.o.d
│   │   │   │   ├── misc/
│   │   │   │   │   ├── unix/
│   │   │   │   │   │   ├── SDL_sysurl.c.o
│   │   │   │   │   │   └── SDL_sysurl.c.o.d
│   │   │   │   │   ├── SDL_libusb.c.o
│   │   │   │   │   ├── SDL_libusb.c.o.d
│   │   │   │   │   ├── SDL_url.c.o
│   │   │   │   │   └── SDL_url.c.o.d
│   │   │   │   ├── notification/
│   │   │   │   │   ├── unix/
│   │   │   │   │   │   ├── SDL_dbusnotification.c.o
│   │   │   │   │   │   └── SDL_dbusnotification.c.o.d
│   │   │   │   │   ├── SDL_notification.c.o
│   │   │   │   │   └── SDL_notification.c.o.d
│   │   │   │   ├── power/
│   │   │   │   │   ├── linux/
│   │   │   │   │   │   ├── SDL_syspower.c.o
│   │   │   │   │   │   └── SDL_syspower.c.o.d
│   │   │   │   │   ├── SDL_power.c.o
│   │   │   │   │   └── SDL_power.c.o.d
│   │   │   │   ├── process/
│   │   │   │   │   ├── posix/
│   │   │   │   │   │   ├── SDL_posixprocess.c.o
│   │   │   │   │   │   └── SDL_posixprocess.c.o.d
│   │   │   │   │   ├── SDL_process.c.o
│   │   │   │   │   └── SDL_process.c.o.d
│   │   │   │   ├── render/
│   │   │   │   │   ├── direct3d/
│   │   │   │   │   │   ├── SDL_render_d3d.c.o
│   │   │   │   │   │   ├── SDL_render_d3d.c.o.d
│   │   │   │   │   │   ├── SDL_shaders_d3d.c.o
│   │   │   │   │   │   └── SDL_shaders_d3d.c.o.d
│   │   │   │   │   ├── direct3d11/
│   │   │   │   │   │   ├── SDL_render_d3d11.c.o
│   │   │   │   │   │   ├── SDL_render_d3d11.c.o.d
│   │   │   │   │   │   ├── SDL_shaders_d3d11.c.o
│   │   │   │   │   │   └── SDL_shaders_d3d11.c.o.d
│   │   │   │   │   ├── direct3d12/
│   │   │   │   │   │   ├── SDL_render_d3d12.c.o
│   │   │   │   │   │   ├── SDL_render_d3d12.c.o.d
│   │   │   │   │   │   ├── SDL_shaders_d3d12.c.o
│   │   │   │   │   │   └── SDL_shaders_d3d12.c.o.d
│   │   │   │   │   ├── gpu/
│   │   │   │   │   │   ├── SDL_pipeline_gpu.c.o
│   │   │   │   │   │   ├── SDL_pipeline_gpu.c.o.d
│   │   │   │   │   │   ├── SDL_render_gpu.c.o
│   │   │   │   │   │   ├── SDL_render_gpu.c.o.d
│   │   │   │   │   │   ├── SDL_shaders_gpu.c.o
│   │   │   │   │   │   └── SDL_shaders_gpu.c.o.d
│   │   │   │   │   ├── ngage/
│   │   │   │   │   │   ├── SDL_render_ngage.c.o
│   │   │   │   │   │   └── SDL_render_ngage.c.o.d
│   │   │   │   │   ├── opengl/
│   │   │   │   │   │   ├── SDL_render_gl.c.o
│   │   │   │   │   │   ├── SDL_render_gl.c.o.d
│   │   │   │   │   │   ├── SDL_shaders_gl.c.o
│   │   │   │   │   │   └── SDL_shaders_gl.c.o.d
│   │   │   │   │   ├── opengles/
│   │   │   │   │   │   ├── SDL_render_gles.c.o
│   │   │   │   │   │   └── SDL_render_gles.c.o.d
│   │   │   │   │   ├── opengles2/
│   │   │   │   │   │   ├── SDL_render_gles2.c.o
│   │   │   │   │   │   ├── SDL_render_gles2.c.o.d
│   │   │   │   │   │   ├── SDL_shaders_gles2.c.o
│   │   │   │   │   │   └── SDL_shaders_gles2.c.o.d
│   │   │   │   │   ├── ps2/
│   │   │   │   │   │   ├── SDL_render_ps2.c.o
│   │   │   │   │   │   └── SDL_render_ps2.c.o.d
│   │   │   │   │   ├── psp/
│   │   │   │   │   │   ├── SDL_render_psp.c.o
│   │   │   │   │   │   └── SDL_render_psp.c.o.d
│   │   │   │   │   ├── software/
│   │   │   │   │   │   ├── SDL_blendfillrect.c.o
│   │   │   │   │   │   ├── SDL_blendfillrect.c.o.d
│   │   │   │   │   │   ├── SDL_blendline.c.o
│   │   │   │   │   │   ├── SDL_blendline.c.o.d
│   │   │   │   │   │   ├── SDL_blendpoint.c.o
│   │   │   │   │   │   ├── SDL_blendpoint.c.o.d
│   │   │   │   │   │   ├── SDL_drawline.c.o
│   │   │   │   │   │   ├── SDL_drawline.c.o.d
│   │   │   │   │   │   ├── SDL_drawpoint.c.o
│   │   │   │   │   │   ├── SDL_drawpoint.c.o.d
│   │   │   │   │   │   ├── SDL_render_sw.c.o
│   │   │   │   │   │   ├── SDL_render_sw.c.o.d
│   │   │   │   │   │   ├── SDL_triangle.c.o
│   │   │   │   │   │   └── SDL_triangle.c.o.d
│   │   │   │   │   ├── vitagxm/
│   │   │   │   │   │   ├── SDL_render_vita_gxm_memory.c.o
│   │   │   │   │   │   ├── SDL_render_vita_gxm_memory.c.o.d
│   │   │   │   │   │   ├── SDL_render_vita_gxm_tools.c.o
│   │   │   │   │   │   ├── SDL_render_vita_gxm_tools.c.o.d
│   │   │   │   │   │   ├── SDL_render_vita_gxm.c.o
│   │   │   │   │   │   └── SDL_render_vita_gxm.c.o.d
│   │   │   │   │   ├── vulkan/
│   │   │   │   │   │   ├── SDL_render_vulkan.c.o
│   │   │   │   │   │   ├── SDL_render_vulkan.c.o.d
│   │   │   │   │   │   ├── SDL_shaders_vulkan.c.o
│   │   │   │   │   │   └── SDL_shaders_vulkan.c.o.d
│   │   │   │   │   ├── SDL_render_unsupported.c.o
│   │   │   │   │   ├── SDL_render_unsupported.c.o.d
│   │   │   │   │   ├── SDL_render.c.o
│   │   │   │   │   ├── SDL_render.c.o.d
│   │   │   │   │   ├── SDL_yuv_sw.c.o
│   │   │   │   │   └── SDL_yuv_sw.c.o.d
│   │   │   │   ├── sensor/
│   │   │   │   │   ├── dummy/
│   │   │   │   │   │   ├── SDL_dummysensor.c.o
│   │   │   │   │   │   └── SDL_dummysensor.c.o.d
│   │   │   │   │   ├── SDL_sensor.c.o
│   │   │   │   │   └── SDL_sensor.c.o.d
│   │   │   │   ├── stdlib/
│   │   │   │   │   ├── SDL_crc16.c.o
│   │   │   │   │   ├── SDL_crc16.c.o.d
│   │   │   │   │   ├── SDL_crc32.c.o
│   │   │   │   │   ├── SDL_crc32.c.o.d
│   │   │   │   │   ├── SDL_getenv.c.o
│   │   │   │   │   ├── SDL_getenv.c.o.d
│   │   │   │   │   ├── SDL_iconv.c.o
│   │   │   │   │   ├── SDL_iconv.c.o.d
│   │   │   │   │   ├── SDL_malloc.c.o
│   │   │   │   │   ├── SDL_malloc.c.o.d
│   │   │   │   │   ├── SDL_memcpy.c.o
│   │   │   │   │   ├── SDL_memcpy.c.o.d
│   │   │   │   │   ├── SDL_memmove.c.o
│   │   │   │   │   ├── SDL_memmove.c.o.d
│   │   │   │   │   ├── SDL_memset.c.o
│   │   │   │   │   ├── SDL_memset.c.o.d
│   │   │   │   │   ├── SDL_mslibc.c.o
│   │   │   │   │   ├── SDL_mslibc.c.o.d
│   │   │   │   │   ├── SDL_murmur3.c.o
│   │   │   │   │   ├── SDL_murmur3.c.o.d
│   │   │   │   │   ├── SDL_qsort.c.o
│   │   │   │   │   ├── SDL_qsort.c.o.d
│   │   │   │   │   ├── SDL_random.c.o
│   │   │   │   │   ├── SDL_random.c.o.d
│   │   │   │   │   ├── SDL_stdlib.c.o
│   │   │   │   │   ├── SDL_stdlib.c.o.d
│   │   │   │   │   ├── SDL_string.c.o
│   │   │   │   │   ├── SDL_string.c.o.d
│   │   │   │   │   ├── SDL_strtokr.c.o
│   │   │   │   │   └── SDL_strtokr.c.o.d
│   │   │   │   ├── storage/
│   │   │   │   │   ├── generic/
│   │   │   │   │   │   ├── SDL_genericstorage.c.o
│   │   │   │   │   │   └── SDL_genericstorage.c.o.d
│   │   │   │   │   ├── steam/
│   │   │   │   │   │   ├── SDL_steamstorage.c.o
│   │   │   │   │   │   └── SDL_steamstorage.c.o.d
│   │   │   │   │   ├── SDL_storage.c.o
│   │   │   │   │   └── SDL_storage.c.o.d
│   │   │   │   ├── thread/
│   │   │   │   │   ├── pthread/
│   │   │   │   │   │   ├── SDL_syscond.c.o
│   │   │   │   │   │   ├── SDL_syscond.c.o.d
│   │   │   │   │   │   ├── SDL_sysmutex.c.o
│   │   │   │   │   │   ├── SDL_sysmutex.c.o.d
│   │   │   │   │   │   ├── SDL_sysrwlock.c.o
│   │   │   │   │   │   ├── SDL_sysrwlock.c.o.d
│   │   │   │   │   │   ├── SDL_syssem.c.o
│   │   │   │   │   │   ├── SDL_syssem.c.o.d
│   │   │   │   │   │   ├── SDL_systhread.c.o
│   │   │   │   │   │   ├── SDL_systhread.c.o.d
│   │   │   │   │   │   ├── SDL_systls.c.o
│   │   │   │   │   │   └── SDL_systls.c.o.d
│   │   │   │   │   ├── SDL_thread.c.o
│   │   │   │   │   └── SDL_thread.c.o.d
│   │   │   │   ├── time/
│   │   │   │   │   ├── unix/
│   │   │   │   │   │   ├── SDL_systime.c.o
│   │   │   │   │   │   └── SDL_systime.c.o.d
│   │   │   │   │   ├── SDL_time.c.o
│   │   │   │   │   └── SDL_time.c.o.d
│   │   │   │   ├── timer/
│   │   │   │   │   ├── unix/
│   │   │   │   │   │   ├── SDL_systimer.c.o
│   │   │   │   │   │   └── SDL_systimer.c.o.d
│   │   │   │   │   ├── SDL_timer.c.o
│   │   │   │   │   └── SDL_timer.c.o.d
│   │   │   │   ├── tray/
│   │   │   │   │   ├── unix/
│   │   │   │   │   │   ├── SDL_dbustray.c.o
│   │   │   │   │   │   ├── SDL_dbustray.c.o.d
│   │   │   │   │   │   ├── SDL_tray.c.o
│   │   │   │   │   │   └── SDL_tray.c.o.d
│   │   │   │   │   ├── SDL_tray_utils.c.o
│   │   │   │   │   └── SDL_tray_utils.c.o.d
│   │   │   │   ├── video/
│   │   │   │   │   ├── dummy/
│   │   │   │   │   │   ├── SDL_nullevents.c.o
│   │   │   │   │   │   ├── SDL_nullevents.c.o.d
│   │   │   │   │   │   ├── SDL_nullframebuffer.c.o
│   │   │   │   │   │   ├── SDL_nullframebuffer.c.o.d
│   │   │   │   │   │   ├── SDL_nullvideo.c.o
│   │   │   │   │   │   └── SDL_nullvideo.c.o.d
│   │   │   │   │   ├── kmsdrm/
│   │   │   │   │   │   ├── SDL_kmsdrmdyn.c.o
│   │   │   │   │   │   ├── SDL_kmsdrmdyn.c.o.d
│   │   │   │   │   │   ├── SDL_kmsdrmevents.c.o
│   │   │   │   │   │   ├── SDL_kmsdrmevents.c.o.d
│   │   │   │   │   │   ├── SDL_kmsdrmmouse.c.o
│   │   │   │   │   │   ├── SDL_kmsdrmmouse.c.o.d
│   │   │   │   │   │   ├── SDL_kmsdrmopengles.c.o
│   │   │   │   │   │   ├── SDL_kmsdrmopengles.c.o.d
│   │   │   │   │   │   ├── SDL_kmsdrmvideo.c.o
│   │   │   │   │   │   ├── SDL_kmsdrmvideo.c.o.d
│   │   │   │   │   │   ├── SDL_kmsdrmvulkan.c.o
│   │   │   │   │   │   └── SDL_kmsdrmvulkan.c.o.d
│   │   │   │   │   ├── offscreen/
│   │   │   │   │   │   ├── SDL_offscreenevents.c.o
│   │   │   │   │   │   ├── SDL_offscreenevents.c.o.d
│   │   │   │   │   │   ├── SDL_offscreenframebuffer.c.o
│   │   │   │   │   │   ├── SDL_offscreenframebuffer.c.o.d
│   │   │   │   │   │   ├── SDL_offscreenopengles.c.o
│   │   │   │   │   │   ├── SDL_offscreenopengles.c.o.d
│   │   │   │   │   │   ├── SDL_offscreenvideo.c.o
│   │   │   │   │   │   ├── SDL_offscreenvideo.c.o.d
│   │   │   │   │   │   ├── SDL_offscreenvulkan.c.o
│   │   │   │   │   │   ├── SDL_offscreenvulkan.c.o.d
│   │   │   │   │   │   ├── SDL_offscreenwindow.c.o
│   │   │   │   │   │   └── SDL_offscreenwindow.c.o.d
│   │   │   │   │   ├── wayland/
│   │   │   │   │   │   ├── SDL_waylandclipboard.c.o
│   │   │   │   │   │   ├── SDL_waylandclipboard.c.o.d
│   │   │   │   │   │   ├── SDL_waylandcolor.c.o
│   │   │   │   │   │   ├── SDL_waylandcolor.c.o.d
│   │   │   │   │   │   ├── SDL_waylanddatamanager.c.o
│   │   │   │   │   │   ├── SDL_waylanddatamanager.c.o.d
│   │   │   │   │   │   ├── SDL_waylanddyn.c.o
│   │   │   │   │   │   ├── SDL_waylanddyn.c.o.d
│   │   │   │   │   │   ├── SDL_waylandevents.c.o
│   │   │   │   │   │   ├── SDL_waylandevents.c.o.d
│   │   │   │   │   │   ├── SDL_waylandeventthread.c.o
│   │   │   │   │   │   ├── SDL_waylandeventthread.c.o.d
│   │   │   │   │   │   ├── SDL_waylandkeyboard.c.o
│   │   │   │   │   │   ├── SDL_waylandkeyboard.c.o.d
│   │   │   │   │   │   ├── SDL_waylandmessagebox.c.o
│   │   │   │   │   │   ├── SDL_waylandmessagebox.c.o.d
│   │   │   │   │   │   ├── SDL_waylandmouse.c.o
│   │   │   │   │   │   ├── SDL_waylandmouse.c.o.d
│   │   │   │   │   │   ├── SDL_waylandopengles.c.o
│   │   │   │   │   │   ├── SDL_waylandopengles.c.o.d
│   │   │   │   │   │   ├── SDL_waylandshmbuffer.c.o
│   │   │   │   │   │   ├── SDL_waylandshmbuffer.c.o.d
│   │   │   │   │   │   ├── SDL_waylandutil.c.o
│   │   │   │   │   │   ├── SDL_waylandutil.c.o.d
│   │   │   │   │   │   ├── SDL_waylandvideo.c.o
│   │   │   │   │   │   ├── SDL_waylandvideo.c.o.d
│   │   │   │   │   │   ├── SDL_waylandvulkan.c.o
│   │   │   │   │   │   ├── SDL_waylandvulkan.c.o.d
│   │   │   │   │   │   ├── SDL_waylandwindow.c.o
│   │   │   │   │   │   └── SDL_waylandwindow.c.o.d
│   │   │   │   │   ├── x11/
│   │   │   │   │   │   ├── edid-parse.c.o
│   │   │   │   │   │   ├── edid-parse.c.o.d
│   │   │   │   │   │   ├── SDL_x11clipboard.c.o
│   │   │   │   │   │   ├── SDL_x11clipboard.c.o.d
│   │   │   │   │   │   ├── SDL_x11dyn.c.o
│   │   │   │   │   │   ├── SDL_x11dyn.c.o.d
│   │   │   │   │   │   ├── SDL_x11events.c.o
│   │   │   │   │   │   ├── SDL_x11events.c.o.d
│   │   │   │   │   │   ├── SDL_x11framebuffer.c.o
│   │   │   │   │   │   ├── SDL_x11framebuffer.c.o.d
│   │   │   │   │   │   ├── SDL_x11keyboard.c.o
│   │   │   │   │   │   ├── SDL_x11keyboard.c.o.d
│   │   │   │   │   │   ├── SDL_x11messagebox.c.o
│   │   │   │   │   │   ├── SDL_x11messagebox.c.o.d
│   │   │   │   │   │   ├── SDL_x11modes.c.o
│   │   │   │   │   │   ├── SDL_x11modes.c.o.d
│   │   │   │   │   │   ├── SDL_x11mouse.c.o
│   │   │   │   │   │   ├── SDL_x11mouse.c.o.d
│   │   │   │   │   │   ├── SDL_x11opengl.c.o
│   │   │   │   │   │   ├── SDL_x11opengl.c.o.d
│   │   │   │   │   │   ├── SDL_x11opengles.c.o
│   │   │   │   │   │   ├── SDL_x11opengles.c.o.d
│   │   │   │   │   │   ├── SDL_x11pen.c.o
│   │   │   │   │   │   ├── SDL_x11pen.c.o.d
│   │   │   │   │   │   ├── SDL_x11settings.c.o
│   │   │   │   │   │   ├── SDL_x11settings.c.o.d
│   │   │   │   │   │   ├── SDL_x11shape.c.o
│   │   │   │   │   │   ├── SDL_x11shape.c.o.d
│   │   │   │   │   │   ├── SDL_x11toolkit.c.o
│   │   │   │   │   │   ├── SDL_x11toolkit.c.o.d
│   │   │   │   │   │   ├── SDL_x11touch.c.o
│   │   │   │   │   │   ├── SDL_x11touch.c.o.d
│   │   │   │   │   │   ├── SDL_x11video.c.o
│   │   │   │   │   │   ├── SDL_x11video.c.o.d
│   │   │   │   │   │   ├── SDL_x11vulkan.c.o
│   │   │   │   │   │   ├── SDL_x11vulkan.c.o.d
│   │   │   │   │   │   ├── SDL_x11window.c.o
│   │   │   │   │   │   ├── SDL_x11window.c.o.d
│   │   │   │   │   │   ├── SDL_x11xfixes.c.o
│   │   │   │   │   │   ├── SDL_x11xfixes.c.o.d
│   │   │   │   │   │   ├── SDL_x11xinput2.c.o
│   │   │   │   │   │   ├── SDL_x11xinput2.c.o.d
│   │   │   │   │   │   ├── SDL_x11xsync.c.o
│   │   │   │   │   │   ├── SDL_x11xsync.c.o.d
│   │   │   │   │   │   ├── SDL_x11xtest.c.o
│   │   │   │   │   │   ├── SDL_x11xtest.c.o.d
│   │   │   │   │   │   ├── xsettings-client.c.o
│   │   │   │   │   │   └── xsettings-client.c.o.d
│   │   │   │   │   ├── yuv2rgb/
│   │   │   │   │   │   ├── yuv_rgb_lsx.c.o
│   │   │   │   │   │   ├── yuv_rgb_lsx.c.o.d
│   │   │   │   │   │   ├── yuv_rgb_sse.c.o
│   │   │   │   │   │   ├── yuv_rgb_sse.c.o.d
│   │   │   │   │   │   ├── yuv_rgb_std.c.o
│   │   │   │   │   │   └── yuv_rgb_std.c.o.d
│   │   │   │   │   ├── SDL_blit_0.c.o
│   │   │   │   │   ├── SDL_blit_0.c.o.d
│   │   │   │   │   ├── SDL_blit_1.c.o
│   │   │   │   │   ├── SDL_blit_1.c.o.d
│   │   │   │   │   ├── SDL_blit_A.c.o
│   │   │   │   │   ├── SDL_blit_A.c.o.d
│   │   │   │   │   ├── SDL_blit_auto.c.o
│   │   │   │   │   ├── SDL_blit_auto.c.o.d
│   │   │   │   │   ├── SDL_blit_copy.c.o
│   │   │   │   │   ├── SDL_blit_copy.c.o.d
│   │   │   │   │   ├── SDL_blit_N.c.o
│   │   │   │   │   ├── SDL_blit_N.c.o.d
│   │   │   │   │   ├── SDL_blit_slow.c.o
│   │   │   │   │   ├── SDL_blit_slow.c.o.d
│   │   │   │   │   ├── SDL_blit.c.o
│   │   │   │   │   ├── SDL_blit.c.o.d
│   │   │   │   │   ├── SDL_bmp.c.o
│   │   │   │   │   ├── SDL_bmp.c.o.d
│   │   │   │   │   ├── SDL_clipboard.c.o
│   │   │   │   │   ├── SDL_clipboard.c.o.d
│   │   │   │   │   ├── SDL_egl.c.o
│   │   │   │   │   ├── SDL_egl.c.o.d
│   │   │   │   │   ├── SDL_fillrect.c.o
│   │   │   │   │   ├── SDL_fillrect.c.o.d
│   │   │   │   │   ├── SDL_pixels.c.o
│   │   │   │   │   ├── SDL_pixels.c.o.d
│   │   │   │   │   ├── SDL_rect.c.o
│   │   │   │   │   ├── SDL_rect.c.o.d
│   │   │   │   │   ├── SDL_RLEaccel.c.o
│   │   │   │   │   ├── SDL_RLEaccel.c.o.d
│   │   │   │   │   ├── SDL_rotate.c.o
│   │   │   │   │   ├── SDL_rotate.c.o.d
│   │   │   │   │   ├── SDL_stb.c.o
│   │   │   │   │   ├── SDL_stb.c.o.d
│   │   │   │   │   ├── SDL_stretch.c.o
│   │   │   │   │   ├── SDL_stretch.c.o.d
│   │   │   │   │   ├── SDL_surface.c.o
│   │   │   │   │   ├── SDL_surface.c.o.d
│   │   │   │   │   ├── SDL_video_unsupported.c.o
│   │   │   │   │   ├── SDL_video_unsupported.c.o.d
│   │   │   │   │   ├── SDL_video.c.o
│   │   │   │   │   ├── SDL_video.c.o.d
│   │   │   │   │   ├── SDL_vulkan_utils.c.o
│   │   │   │   │   ├── SDL_vulkan_utils.c.o.d
│   │   │   │   │   ├── SDL_yuv.c.o
│   │   │   │   │   └── SDL_yuv.c.o.d
│   │   │   │   ├── SDL_assert.c.o
│   │   │   │   ├── SDL_assert.c.o.d
│   │   │   │   ├── SDL_error.c.o
│   │   │   │   ├── SDL_error.c.o.d
│   │   │   │   ├── SDL_guid.c.o
│   │   │   │   ├── SDL_guid.c.o.d
│   │   │   │   ├── SDL_hashtable.c.o
│   │   │   │   ├── SDL_hashtable.c.o.d
│   │   │   │   ├── SDL_hints.c.o
│   │   │   │   ├── SDL_hints.c.o.d
│   │   │   │   ├── SDL_list.c.o
│   │   │   │   ├── SDL_list.c.o.d
│   │   │   │   ├── SDL_log.c.o
│   │   │   │   ├── SDL_log.c.o.d
│   │   │   │   ├── SDL_properties.c.o
│   │   │   │   ├── SDL_properties.c.o.d
│   │   │   │   ├── SDL_utils.c.o
│   │   │   │   ├── SDL_utils.c.o.d
│   │   │   │   ├── SDL.c.o
│   │   │   │   └── SDL.c.o.d
│   │   │   ├── wayland-generated-protocols/
│   │   │   │   ├── alpha-modifier-v1-protocol.c.o
│   │   │   │   ├── alpha-modifier-v1-protocol.c.o.d
│   │   │   │   ├── color-management-v1-protocol.c.o
│   │   │   │   ├── color-management-v1-protocol.c.o.d
│   │   │   │   ├── cursor-shape-v1-protocol.c.o
│   │   │   │   ├── cursor-shape-v1-protocol.c.o.d
│   │   │   │   ├── fractional-scale-v1-protocol.c.o
│   │   │   │   ├── fractional-scale-v1-protocol.c.o.d
│   │   │   │   ├── frog-color-management-v1-protocol.c.o
│   │   │   │   ├── frog-color-management-v1-protocol.c.o.d
│   │   │   │   ├── idle-inhibit-unstable-v1-protocol.c.o
│   │   │   │   ├── idle-inhibit-unstable-v1-protocol.c.o.d
│   │   │   │   ├── input-timestamps-unstable-v1-protocol.c.o
│   │   │   │   ├── input-timestamps-unstable-v1-protocol.c.o.d
│   │   │   │   ├── keyboard-shortcuts-inhibit-unstable-v1-protocol.c.o
│   │   │   │   ├── keyboard-shortcuts-inhibit-unstable-v1-protocol.c.o.d
│   │   │   │   ├── pointer-constraints-unstable-v1-protocol.c.o
│   │   │   │   ├── pointer-constraints-unstable-v1-protocol.c.o.d
│   │   │   │   ├── pointer-gestures-unstable-v1-protocol.c.o
│   │   │   │   ├── pointer-gestures-unstable-v1-protocol.c.o.d
│   │   │   │   ├── pointer-warp-v1-protocol.c.o
│   │   │   │   ├── pointer-warp-v1-protocol.c.o.d
│   │   │   │   ├── primary-selection-unstable-v1-protocol.c.o
│   │   │   │   ├── primary-selection-unstable-v1-protocol.c.o.d
│   │   │   │   ├── relative-pointer-unstable-v1-protocol.c.o
│   │   │   │   ├── relative-pointer-unstable-v1-protocol.c.o.d
│   │   │   │   ├── single-pixel-buffer-v1-protocol.c.o
│   │   │   │   ├── single-pixel-buffer-v1-protocol.c.o.d
│   │   │   │   ├── tablet-v2-protocol.c.o
│   │   │   │   ├── tablet-v2-protocol.c.o.d
│   │   │   │   ├── text-input-unstable-v3-protocol.c.o
│   │   │   │   ├── text-input-unstable-v3-protocol.c.o.d
│   │   │   │   ├── viewporter-protocol.c.o
│   │   │   │   ├── viewporter-protocol.c.o.d
│   │   │   │   ├── wayland-protocol.c.o
│   │   │   │   ├── wayland-protocol.c.o.d
│   │   │   │   ├── xdg-activation-v1-protocol.c.o
│   │   │   │   ├── xdg-activation-v1-protocol.c.o.d
│   │   │   │   ├── xdg-decoration-unstable-v1-protocol.c.o
│   │   │   │   ├── xdg-decoration-unstable-v1-protocol.c.o.d
│   │   │   │   ├── xdg-dialog-v1-protocol.c.o
│   │   │   │   ├── xdg-dialog-v1-protocol.c.o.d
│   │   │   │   ├── xdg-foreign-unstable-v2-protocol.c.o
│   │   │   │   ├── xdg-foreign-unstable-v2-protocol.c.o.d
│   │   │   │   ├── xdg-output-unstable-v1-protocol.c.o
│   │   │   │   ├── xdg-output-unstable-v1-protocol.c.o.d
│   │   │   │   ├── xdg-session-management-v1-protocol.c.o
│   │   │   │   ├── xdg-session-management-v1-protocol.c.o.d
│   │   │   │   ├── xdg-shell-protocol.c.o
│   │   │   │   ├── xdg-shell-protocol.c.o.d
│   │   │   │   ├── xdg-toplevel-icon-v1-protocol.c.o
│   │   │   │   ├── xdg-toplevel-icon-v1-protocol.c.o.d
│   │   │   │   ├── xdg-toplevel-tag-v1-protocol.c.o
│   │   │   │   └── xdg-toplevel-tag-v1-protocol.c.o.d
│   │   │   ├── build.make
│   │   │   ├── cmake_clean.cmake
│   │   │   ├── cmake_pch.h
│   │   │   ├── cmake_pch.h.c
│   │   │   ├── cmake_pch.h.gch
│   │   │   ├── cmake_pch.h.gch.d
│   │   │   ├── compiler_depend.internal
│   │   │   ├── compiler_depend.make
│   │   │   ├── compiler_depend.ts
│   │   │   ├── depend.make
│   │   │   ├── DependInfo.cmake
│   │   │   ├── flags.make
│   │   │   ├── link.txt
│   │   │   └── progress.make
│   │   ├── uninstall.dir/
│   │   │   ├── build.make
│   │   │   ├── cmake_clean.cmake
│   │   │   ├── compiler_depend.make
│   │   │   ├── compiler_depend.ts
│   │   │   ├── DependInfo.cmake
│   │   │   └── progress.make
│   │   ├── cmake.check_cache
│   │   ├── cmake.verify_globs
│   │   ├── CMakeConfigureLog.yaml
│   │   ├── CMakeDirectoryInformation.cmake
│   │   ├── CMakeRuleHashes.txt
│   │   ├── dummy.sym
│   │   ├── Makefile.cmake
│   │   ├── Makefile2
│   │   ├── progress.marks
│   │   ├── SDL_build_config.h.intermediate
│   │   ├── TargetDirectories.txt
│   │   └── VerifyGlobs.cmake
│   ├── include-config-relwithdebinfo/
│   │   └── build_config/
│   │       └── SDL_build_config.h
│   ├── include-revision/
│   │   └── SDL3/
│   │       └── SDL_revision.h
│   ├── test/
│   │   ├── CMakeFiles/
│   │   │   ├── CheckCXX/
│   │   │   │   ├── CMakeFiles/
│   │   │   │   │   ├── 3.28.3/
│   │   │   │   │   │   ├── CompilerIdCXX/
│   │   │   │   │   │   │   ├── tmp/
│   │   │   │   │   │   │   ├── a.out
│   │   │   │   │   │   │   └── CMakeCXXCompilerId.cpp
│   │   │   │   │   │   ├── CMakeCXXCompiler.cmake
│   │   │   │   │   │   ├── CMakeDetermineCompilerABI_CXX.bin
│   │   │   │   │   │   └── CMakeSystem.cmake
│   │   │   │   │   ├── CMakeScratch/
│   │   │   │   │   ├── pkgRedirects/
│   │   │   │   │   ├── cmake.check_cache
│   │   │   │   │   ├── CMakeConfigureLog.yaml
│   │   │   │   │   ├── CMakeDirectoryInformation.cmake
│   │   │   │   │   ├── Makefile.cmake
│   │   │   │   │   ├── Makefile2
│   │   │   │   │   ├── progress.marks
│   │   │   │   │   └── TargetDirectories.txt
│   │   │   │   ├── cmake_install.cmake
│   │   │   │   ├── CMakeCache.txt
│   │   │   │   ├── CMakeLists.txt
│   │   │   │   ├── Makefile
│   │   │   │   └── result.cmake
│   │   │   ├── checkkeys.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   └── progress.make
│   │   │   ├── childprocess.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   └── progress.make
│   │   │   ├── copy-sdl-test-resources.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   └── progress.make
│   │   │   ├── generate-gamepad_image_headers.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   └── progress.make
│   │   │   ├── generate-glass_png_header.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   └── progress.make
│   │   │   ├── generate-icon_png_header.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   └── progress.make
│   │   │   ├── loopwave.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   └── progress.make
│   │   │   ├── sdltests_utils.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   └── progress.make
│   │   │   ├── testasyncio.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   └── progress.make
│   │   │   ├── testatomic.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   └── progress.make
│   │   │   ├── testaudio.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   └── progress.make
│   │   │   ├── testaudiohotplug.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   └── progress.make
│   │   │   ├── testaudioinfo.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   └── progress.make
│   │   │   ├── testaudiorecording.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   └── progress.make
│   │   │   ├── testaudiostreamdynamicresample.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   └── progress.make
│   │   │   ├── testautomation.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   └── progress.make
│   │   │   ├── testbounds.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   └── progress.make
│   │   │   ├── testcamera.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   └── progress.make
│   │   │   ├── testclipboard.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   └── progress.make
│   │   │   ├── testcolorspace.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   └── progress.make
│   │   │   ├── testcontroller.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   └── progress.make
│   │   │   ├── testcustomcursor.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   └── progress.make
│   │   │   ├── testdescriptor.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   └── progress.make
│   │   │   ├── testdialog.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   └── progress.make
│   │   │   ├── testdisplayinfo.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   └── progress.make
│   │   │   ├── testdlopennote.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   └── progress.make
│   │   │   ├── testdraw.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   └── progress.make
│   │   │   ├── testdrawchessboard.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   └── progress.make
│   │   │   ├── testdropfile.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   └── progress.make
│   │   │   ├── testdynaudioreopen.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   └── progress.make
│   │   │   ├── testerror.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   └── progress.make
│   │   │   ├── testevdev.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   └── progress.make
│   │   │   ├── testfile.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   └── progress.make
│   │   │   ├── testfilesystem.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   └── progress.make
│   │   │   ├── testgeometry.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   └── progress.make
│   │   │   ├── testgl.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   └── progress.make
│   │   │   ├── testgles.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   └── progress.make
│   │   │   ├── testgles2.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   └── progress.make
│   │   │   ├── testgpu_simple_clear.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   └── progress.make
│   │   │   ├── testgpu_spinning_cube_xr.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   └── progress.make
│   │   │   ├── testgpu_spinning_cube.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   └── progress.make
│   │   │   ├── testgpurender_effects.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   └── progress.make
│   │   │   ├── testgpurender_msdf.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   └── progress.make
│   │   │   ├── testhaptic.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   └── progress.make
│   │   │   ├── testhittesting.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   └── progress.make
│   │   │   ├── testhotplug.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   └── progress.make
│   │   │   ├── testiconv.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   └── progress.make
│   │   │   ├── testime.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   └── progress.make
│   │   │   ├── testintersections.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   └── progress.make
│   │   │   ├── testkeys.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   └── progress.make
│   │   │   ├── testloadso.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   └── progress.make
│   │   │   ├── testlocale.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   └── progress.make
│   │   │   ├── testlock.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   └── progress.make
│   │   │   ├── testmanymouse.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   └── progress.make
│   │   │   ├── testmessage.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   └── progress.make
│   │   │   ├── testmodal.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   └── progress.make
│   │   │   ├── testmouse.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   └── progress.make
│   │   │   ├── testmultiaudio.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   └── progress.make
│   │   │   ├── testnative.dir/
│   │   │   │   ├── __/
│   │   │   │   │   └── wayland-generated-protocols/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   └── progress.make
│   │   │   ├── testnotification.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   └── progress.make
│   │   │   ├── testoffscreen.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   └── progress.make
│   │   │   ├── testoverlay.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   └── progress.make
│   │   │   ├── testpalette.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   └── progress.make
│   │   │   ├── testpen.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   └── progress.make
│   │   │   ├── testplatform.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   └── progress.make
│   │   │   ├── testpopup.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   └── progress.make
│   │   │   ├── testpower.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   └── progress.make
│   │   │   ├── testprocess.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   └── progress.make
│   │   │   ├── testqsort.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   └── progress.make
│   │   │   ├── testrelative.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   └── progress.make
│   │   │   ├── testrendercopyex.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   └── progress.make
│   │   │   ├── testrendertarget.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   └── progress.make
│   │   │   ├── testresample.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   └── progress.make
│   │   │   ├── testrotate.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   └── progress.make
│   │   │   ├── testrumble.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   └── progress.make
│   │   │   ├── testrwlock.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   └── progress.make
│   │   │   ├── testscale.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   └── progress.make
│   │   │   ├── testsem.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   └── progress.make
│   │   │   ├── testsensor.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   └── progress.make
│   │   │   ├── testshader.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   └── progress.make
│   │   │   ├── testshape.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   └── progress.make
│   │   │   ├── testsoftwaretransparent.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   └── progress.make
│   │   │   ├── testsprite.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   └── progress.make
│   │   │   ├── testspritecxx.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   └── progress.make
│   │   │   ├── testspriteminimal.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   └── progress.make
│   │   │   ├── testspritesurface.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   └── progress.make
│   │   │   ├── testsurround.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   └── progress.make
│   │   │   ├── testsymbols.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   └── progress.make
│   │   │   ├── testthread.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   └── progress.make
│   │   │   ├── testtime.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   └── progress.make
│   │   │   ├── testtimer.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   └── progress.make
│   │   │   ├── testtray.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   └── progress.make
│   │   │   ├── testurl.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   └── progress.make
│   │   │   ├── testver.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   └── progress.make
│   │   │   ├── testviewport.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   └── progress.make
│   │   │   ├── testvulkan.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   └── progress.make
│   │   │   ├── testwaylandcustom.dir/
│   │   │   │   ├── __/
│   │   │   │   │   └── wayland-generated-protocols/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   └── progress.make
│   │   │   ├── testwm.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   └── progress.make
│   │   │   ├── testx11windowdestroy.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.internal
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   ├── progress.make
│   │   │   │   ├── testx11windowdestroy.c.o
│   │   │   │   └── testx11windowdestroy.c.o.d
│   │   │   ├── testyuv.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   └── progress.make
│   │   │   ├── torturethread.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   └── progress.make
│   │   │   ├── CMakeDirectoryInformation.cmake
│   │   │   └── progress.marks
│   │   ├── cmake_install.cmake
│   │   ├── CTestTestfile.cmake
│   │   ├── Makefile
│   │   └── testx11windowdestroy
│   ├── Testing/
│   │   └── Temporary/
│   │       ├── CTestCostData.txt
│   │       └── LastTest.log
│   ├── wayland-generated-protocols/
│   │   ├── alpha-modifier-v1-client-protocol.h
│   │   ├── alpha-modifier-v1-protocol.c
│   │   ├── color-management-v1-client-protocol.h
│   │   ├── color-management-v1-protocol.c
│   │   ├── cursor-shape-v1-client-protocol.h
│   │   ├── cursor-shape-v1-protocol.c
│   │   ├── fractional-scale-v1-client-protocol.h
│   │   ├── fractional-scale-v1-protocol.c
│   │   ├── frog-color-management-v1-client-protocol.h
│   │   ├── frog-color-management-v1-protocol.c
│   │   ├── idle-inhibit-unstable-v1-client-protocol.h
│   │   ├── idle-inhibit-unstable-v1-protocol.c
│   │   ├── input-timestamps-unstable-v1-client-protocol.h
│   │   ├── input-timestamps-unstable-v1-protocol.c
│   │   ├── keyboard-shortcuts-inhibit-unstable-v1-client-protocol.h
│   │   ├── keyboard-shortcuts-inhibit-unstable-v1-protocol.c
│   │   ├── pointer-constraints-unstable-v1-client-protocol.h
│   │   ├── pointer-constraints-unstable-v1-protocol.c
│   │   ├── pointer-gestures-unstable-v1-client-protocol.h
│   │   ├── pointer-gestures-unstable-v1-protocol.c
│   │   ├── pointer-warp-v1-client-protocol.h
│   │   ├── pointer-warp-v1-protocol.c
│   │   ├── primary-selection-unstable-v1-client-protocol.h
│   │   ├── primary-selection-unstable-v1-protocol.c
│   │   ├── relative-pointer-unstable-v1-client-protocol.h
│   │   ├── relative-pointer-unstable-v1-protocol.c
│   │   ├── single-pixel-buffer-v1-client-protocol.h
│   │   ├── single-pixel-buffer-v1-protocol.c
│   │   ├── tablet-v2-client-protocol.h
│   │   ├── tablet-v2-protocol.c
│   │   ├── text-input-unstable-v3-client-protocol.h
│   │   ├── text-input-unstable-v3-protocol.c
│   │   ├── viewporter-client-protocol.h
│   │   ├── viewporter-protocol.c
│   │   ├── wayland-client-protocol.h
│   │   ├── wayland-protocol.c
│   │   ├── xdg-activation-v1-client-protocol.h
│   │   ├── xdg-activation-v1-protocol.c
│   │   ├── xdg-decoration-unstable-v1-client-protocol.h
│   │   ├── xdg-decoration-unstable-v1-protocol.c
│   │   ├── xdg-dialog-v1-client-protocol.h
│   │   ├── xdg-dialog-v1-protocol.c
│   │   ├── xdg-foreign-unstable-v2-client-protocol.h
│   │   ├── xdg-foreign-unstable-v2-protocol.c
│   │   ├── xdg-output-unstable-v1-client-protocol.h
│   │   ├── xdg-output-unstable-v1-protocol.c
│   │   ├── xdg-session-management-v1-client-protocol.h
│   │   ├── xdg-session-management-v1-protocol.c
│   │   ├── xdg-shell-client-protocol.h
│   │   ├── xdg-shell-protocol.c
│   │   ├── xdg-toplevel-icon-v1-client-protocol.h
│   │   ├── xdg-toplevel-icon-v1-protocol.c
│   │   ├── xdg-toplevel-tag-v1-client-protocol.h
│   │   └── xdg-toplevel-tag-v1-protocol.c
│   ├── cmake_install.cmake
│   ├── cmake_uninstall.cmake
│   ├── CMakeCache.txt
│   ├── CPackConfig.cmake
│   ├── CPackProjectConfig.cmake
│   ├── CPackSourceConfig.cmake
│   ├── CTestTestfile.cmake
│   ├── libSDL_uclibc.a
│   ├── libSDL3_test.a
│   ├── libSDL3.so.0.5.0
│   ├── Makefile
│   ├── sdl3.pc
│   ├── SDL3Config.cmake
│   ├── SDL3ConfigVersion.cmake
│   ├── SDL3headersTargets.cmake
│   ├── SDL3sharedTargets.cmake
│   └── SDL3testTargets.cmake
├── build-no-xi/
│   ├── CMakeFiles/
│   │   ├── 3.28.3/
│   │   │   ├── CompilerIdC/
│   │   │   │   ├── tmp/
│   │   │   │   ├── a.out
│   │   │   │   └── CMakeCCompilerId.c
│   │   │   ├── CompilerIdCXX/
│   │   │   │   ├── tmp/
│   │   │   │   ├── a.out
│   │   │   │   └── CMakeCXXCompilerId.cpp
│   │   │   ├── CMakeCCompiler.cmake
│   │   │   ├── CMakeCXXCompiler.cmake
│   │   │   ├── CMakeDetermineCompilerABI_C.bin
│   │   │   ├── CMakeDetermineCompilerABI_CXX.bin
│   │   │   └── CMakeSystem.cmake
│   │   ├── CMakeScratch/
│   │   ├── CMakeTmp/
│   │   │   ├── SDL_detect_arch/
│   │   │   │   ├── CMakeFiles/
│   │   │   │   │   └── CMakeTmp/
│   │   │   │   └── bin
│   │   │   └── SDL_detect_arch.c
│   │   ├── Export/
│   │   │   └── 35815d1d52a6ea1175d74784b559bdb6/
│   │   │       ├── SDL3headersTargets.cmake
│   │   │       ├── SDL3sharedTargets-relwithdebinfo.cmake
│   │   │       ├── SDL3sharedTargets.cmake
│   │   │       ├── SDL3testTargets-relwithdebinfo.cmake
│   │   │       └── SDL3testTargets.cmake
│   │   ├── git-data/
│   │   │   ├── grabRef.cmake
│   │   │   ├── HEAD
│   │   │   └── head-ref
│   │   ├── pkgRedirects/
│   │   ├── SDL_uclibc.dir/
│   │   │   ├── src/
│   │   │   │   └── libm/
│   │   │   │       ├── e_atan2.c.o
│   │   │   │       ├── e_atan2.c.o.d
│   │   │   │       ├── e_exp.c.o
│   │   │   │       ├── e_exp.c.o.d
│   │   │   │       ├── e_fmod.c.o
│   │   │   │       ├── e_fmod.c.o.d
│   │   │   │       ├── e_log.c.o
│   │   │   │       ├── e_log.c.o.d
│   │   │   │       ├── e_log10.c.o
│   │   │   │       ├── e_log10.c.o.d
│   │   │   │       ├── e_pow.c.o
│   │   │   │       ├── e_pow.c.o.d
│   │   │   │       ├── e_rem_pio2.c.o
│   │   │   │       ├── e_rem_pio2.c.o.d
│   │   │   │       ├── e_sqrt.c.o
│   │   │   │       ├── e_sqrt.c.o.d
│   │   │   │       ├── k_cos.c.o
│   │   │   │       ├── k_cos.c.o.d
│   │   │   │       ├── k_rem_pio2.c.o
│   │   │   │       ├── k_rem_pio2.c.o.d
│   │   │   │       ├── k_sin.c.o
│   │   │   │       ├── k_sin.c.o.d
│   │   │   │       ├── k_tan.c.o
│   │   │   │       ├── k_tan.c.o.d
│   │   │   │       ├── s_atan.c.o
│   │   │   │       ├── s_atan.c.o.d
│   │   │   │       ├── s_copysign.c.o
│   │   │   │       ├── s_copysign.c.o.d
│   │   │   │       ├── s_cos.c.o
│   │   │   │       ├── s_cos.c.o.d
│   │   │   │       ├── s_fabs.c.o
│   │   │   │       ├── s_fabs.c.o.d
│   │   │   │       ├── s_floor.c.o
│   │   │   │       ├── s_floor.c.o.d
│   │   │   │       ├── s_isinf.c.o
│   │   │   │       ├── s_isinf.c.o.d
│   │   │   │       ├── s_isinff.c.o
│   │   │   │       ├── s_isinff.c.o.d
│   │   │   │       ├── s_isnan.c.o
│   │   │   │       ├── s_isnan.c.o.d
│   │   │   │       ├── s_isnanf.c.o
│   │   │   │       ├── s_isnanf.c.o.d
│   │   │   │       ├── s_modf.c.o
│   │   │   │       ├── s_modf.c.o.d
│   │   │   │       ├── s_scalbn.c.o
│   │   │   │       ├── s_scalbn.c.o.d
│   │   │   │       ├── s_sin.c.o
│   │   │   │       ├── s_sin.c.o.d
│   │   │   │       ├── s_tan.c.o
│   │   │   │       └── s_tan.c.o.d
│   │   │   ├── build.make
│   │   │   ├── cmake_clean_target.cmake
│   │   │   ├── cmake_clean.cmake
│   │   │   ├── compiler_depend.internal
│   │   │   ├── compiler_depend.make
│   │   │   ├── compiler_depend.ts
│   │   │   ├── depend.make
│   │   │   ├── DependInfo.cmake
│   │   │   ├── flags.make
│   │   │   ├── link.txt
│   │   │   └── progress.make
│   │   ├── SDL3_test.dir/
│   │   │   ├── src/
│   │   │   │   └── test/
│   │   │   │       ├── SDL_test_assert.c.o
│   │   │   │       ├── SDL_test_assert.c.o.d
│   │   │   │       ├── SDL_test_common.c.o
│   │   │   │       ├── SDL_test_common.c.o.d
│   │   │   │       ├── SDL_test_compare.c.o
│   │   │   │       ├── SDL_test_compare.c.o.d
│   │   │   │       ├── SDL_test_crc32.c.o
│   │   │   │       ├── SDL_test_crc32.c.o.d
│   │   │   │       ├── SDL_test_font.c.o
│   │   │   │       ├── SDL_test_font.c.o.d
│   │   │   │       ├── SDL_test_fuzzer.c.o
│   │   │   │       ├── SDL_test_fuzzer.c.o.d
│   │   │   │       ├── SDL_test_harness.c.o
│   │   │   │       ├── SDL_test_harness.c.o.d
│   │   │   │       ├── SDL_test_log.c.o
│   │   │   │       ├── SDL_test_log.c.o.d
│   │   │   │       ├── SDL_test_md5.c.o
│   │   │   │       ├── SDL_test_md5.c.o.d
│   │   │   │       ├── SDL_test_memory.c.o
│   │   │   │       └── SDL_test_memory.c.o.d
│   │   │   ├── build.make
│   │   │   ├── cmake_clean_target.cmake
│   │   │   ├── cmake_clean.cmake
│   │   │   ├── compiler_depend.internal
│   │   │   ├── compiler_depend.make
│   │   │   ├── compiler_depend.ts
│   │   │   ├── depend.make
│   │   │   ├── DependInfo.cmake
│   │   │   ├── flags.make
│   │   │   ├── link.txt
│   │   │   └── progress.make
│   │   ├── SDL3-shared.dir/
│   │   │   ├── src/
│   │   │   │   ├── atomic/
│   │   │   │   │   ├── SDL_atomic.c.o
│   │   │   │   │   ├── SDL_atomic.c.o.d
│   │   │   │   │   ├── SDL_spinlock.c.o
│   │   │   │   │   └── SDL_spinlock.c.o.d
│   │   │   │   ├── audio/
│   │   │   │   │   ├── alsa/
│   │   │   │   │   │   ├── SDL_alsa_audio.c.o
│   │   │   │   │   │   └── SDL_alsa_audio.c.o.d
│   │   │   │   │   ├── disk/
│   │   │   │   │   │   ├── SDL_diskaudio.c.o
│   │   │   │   │   │   └── SDL_diskaudio.c.o.d
│   │   │   │   │   ├── dummy/
│   │   │   │   │   │   ├── SDL_dummyaudio.c.o
│   │   │   │   │   │   └── SDL_dummyaudio.c.o.d
│   │   │   │   │   ├── pipewire/
│   │   │   │   │   │   ├── SDL_pipewire.c.o
│   │   │   │   │   │   └── SDL_pipewire.c.o.d
│   │   │   │   │   ├── pulseaudio/
│   │   │   │   │   │   ├── SDL_pulseaudio.c.o
│   │   │   │   │   │   └── SDL_pulseaudio.c.o.d
│   │   │   │   │   ├── SDL_audio.c.o
│   │   │   │   │   ├── SDL_audio.c.o.d
│   │   │   │   │   ├── SDL_audiocvt.c.o
│   │   │   │   │   ├── SDL_audiocvt.c.o.d
│   │   │   │   │   ├── SDL_audiodev.c.o
│   │   │   │   │   ├── SDL_audiodev.c.o.d
│   │   │   │   │   ├── SDL_audioqueue.c.o
│   │   │   │   │   ├── SDL_audioqueue.c.o.d
│   │   │   │   │   ├── SDL_audioresample.c.o
│   │   │   │   │   ├── SDL_audioresample.c.o.d
│   │   │   │   │   ├── SDL_audiotypecvt.c.o
│   │   │   │   │   ├── SDL_audiotypecvt.c.o.d
│   │   │   │   │   ├── SDL_mixer.c.o
│   │   │   │   │   ├── SDL_mixer.c.o.d
│   │   │   │   │   ├── SDL_wave.c.o
│   │   │   │   │   └── SDL_wave.c.o.d
│   │   │   │   ├── camera/
│   │   │   │   │   ├── dummy/
│   │   │   │   │   │   ├── SDL_camera_dummy.c.o
│   │   │   │   │   │   └── SDL_camera_dummy.c.o.d
│   │   │   │   │   ├── pipewire/
│   │   │   │   │   │   ├── SDL_camera_pipewire.c.o
│   │   │   │   │   │   └── SDL_camera_pipewire.c.o.d
│   │   │   │   │   ├── v4l2/
│   │   │   │   │   │   ├── SDL_camera_v4l2.c.o
│   │   │   │   │   │   └── SDL_camera_v4l2.c.o.d
│   │   │   │   │   ├── SDL_camera.c.o
│   │   │   │   │   └── SDL_camera.c.o.d
│   │   │   │   ├── core/
│   │   │   │   │   ├── linux/
│   │   │   │   │   │   ├── SDL_dbus.c.o
│   │   │   │   │   │   ├── SDL_dbus.c.o.d
│   │   │   │   │   │   ├── SDL_evdev_capabilities.c.o
│   │   │   │   │   │   ├── SDL_evdev_capabilities.c.o.d
│   │   │   │   │   │   ├── SDL_evdev_kbd.c.o
│   │   │   │   │   │   ├── SDL_evdev_kbd.c.o.d
│   │   │   │   │   │   ├── SDL_evdev.c.o
│   │   │   │   │   │   ├── SDL_evdev.c.o.d
│   │   │   │   │   │   ├── SDL_fcitx.c.o
│   │   │   │   │   │   ├── SDL_fcitx.c.o.d
│   │   │   │   │   │   ├── SDL_ibus.c.o
│   │   │   │   │   │   ├── SDL_ibus.c.o.d
│   │   │   │   │   │   ├── SDL_ime.c.o
│   │   │   │   │   │   ├── SDL_ime.c.o.d
│   │   │   │   │   │   ├── SDL_progressbar.c.o
│   │   │   │   │   │   ├── SDL_progressbar.c.o.d
│   │   │   │   │   │   ├── SDL_system_theme.c.o
│   │   │   │   │   │   ├── SDL_system_theme.c.o.d
│   │   │   │   │   │   ├── SDL_threadprio.c.o
│   │   │   │   │   │   ├── SDL_threadprio.c.o.d
│   │   │   │   │   │   ├── SDL_ubuntu_touch.c.o
│   │   │   │   │   │   ├── SDL_ubuntu_touch.c.o.d
│   │   │   │   │   │   ├── SDL_udev.c.o
│   │   │   │   │   │   └── SDL_udev.c.o.d
│   │   │   │   │   ├── unix/
│   │   │   │   │   │   ├── SDL_appid.c.o
│   │   │   │   │   │   ├── SDL_appid.c.o.d
│   │   │   │   │   │   ├── SDL_fribidi.c.o
│   │   │   │   │   │   ├── SDL_fribidi.c.o.d
│   │   │   │   │   │   ├── SDL_libthai.c.o
│   │   │   │   │   │   ├── SDL_libthai.c.o.d
│   │   │   │   │   │   ├── SDL_poll.c.o
│   │   │   │   │   │   └── SDL_poll.c.o.d
│   │   │   │   │   ├── SDL_core_unsupported.c.o
│   │   │   │   │   └── SDL_core_unsupported.c.o.d
│   │   │   │   ├── cpuinfo/
│   │   │   │   │   ├── SDL_cpuinfo.c.o
│   │   │   │   │   └── SDL_cpuinfo.c.o.d
│   │   │   │   ├── dialog/
│   │   │   │   │   ├── unix/
│   │   │   │   │   │   ├── SDL_portaldialog.c.o
│   │   │   │   │   │   ├── SDL_portaldialog.c.o.d
│   │   │   │   │   │   ├── SDL_unixdialog.c.o
│   │   │   │   │   │   ├── SDL_unixdialog.c.o.d
│   │   │   │   │   │   ├── SDL_zenitydialog.c.o
│   │   │   │   │   │   ├── SDL_zenitydialog.c.o.d
│   │   │   │   │   │   ├── SDL_zenitymessagebox.c.o
│   │   │   │   │   │   └── SDL_zenitymessagebox.c.o.d
│   │   │   │   │   ├── SDL_dialog_utils.c.o
│   │   │   │   │   ├── SDL_dialog_utils.c.o.d
│   │   │   │   │   ├── SDL_dialog.c.o
│   │   │   │   │   └── SDL_dialog.c.o.d
│   │   │   │   ├── dynapi/
│   │   │   │   │   ├── SDL_dynapi.c.o
│   │   │   │   │   └── SDL_dynapi.c.o.d
│   │   │   │   ├── events/
│   │   │   │   │   ├── imKStoUCS.c.o
│   │   │   │   │   ├── imKStoUCS.c.o.d
│   │   │   │   │   ├── SDL_categories.c.o
│   │   │   │   │   ├── SDL_categories.c.o.d
│   │   │   │   │   ├── SDL_clipboardevents.c.o
│   │   │   │   │   ├── SDL_clipboardevents.c.o.d
│   │   │   │   │   ├── SDL_displayevents.c.o
│   │   │   │   │   ├── SDL_displayevents.c.o.d
│   │   │   │   │   ├── SDL_dropevents.c.o
│   │   │   │   │   ├── SDL_dropevents.c.o.d
│   │   │   │   │   ├── SDL_events.c.o
│   │   │   │   │   ├── SDL_events.c.o.d
│   │   │   │   │   ├── SDL_eventwatch.c.o
│   │   │   │   │   ├── SDL_eventwatch.c.o.d
│   │   │   │   │   ├── SDL_keyboard.c.o
│   │   │   │   │   ├── SDL_keyboard.c.o.d
│   │   │   │   │   ├── SDL_keymap.c.o
│   │   │   │   │   ├── SDL_keymap.c.o.d
│   │   │   │   │   ├── SDL_keysym_to_keycode.c.o
│   │   │   │   │   ├── SDL_keysym_to_keycode.c.o.d
│   │   │   │   │   ├── SDL_keysym_to_scancode.c.o
│   │   │   │   │   ├── SDL_keysym_to_scancode.c.o.d
│   │   │   │   │   ├── SDL_mouse.c.o
│   │   │   │   │   ├── SDL_mouse.c.o.d
│   │   │   │   │   ├── SDL_notificationevents.c.o
│   │   │   │   │   ├── SDL_notificationevents.c.o.d
│   │   │   │   │   ├── SDL_pen.c.o
│   │   │   │   │   ├── SDL_pen.c.o.d
│   │   │   │   │   ├── SDL_quit.c.o
│   │   │   │   │   ├── SDL_quit.c.o.d
│   │   │   │   │   ├── SDL_scancode_tables.c.o
│   │   │   │   │   ├── SDL_scancode_tables.c.o.d
│   │   │   │   │   ├── SDL_touch.c.o
│   │   │   │   │   ├── SDL_touch.c.o.d
│   │   │   │   │   ├── SDL_windowevents.c.o
│   │   │   │   │   └── SDL_windowevents.c.o.d
│   │   │   │   ├── filesystem/
│   │   │   │   │   ├── posix/
│   │   │   │   │   │   ├── SDL_sysfsops.c.o
│   │   │   │   │   │   └── SDL_sysfsops.c.o.d
│   │   │   │   │   ├── unix/
│   │   │   │   │   │   ├── SDL_sysfilesystem.c.o
│   │   │   │   │   │   └── SDL_sysfilesystem.c.o.d
│   │   │   │   │   ├── SDL_filesystem.c.o
│   │   │   │   │   └── SDL_filesystem.c.o.d
│   │   │   │   ├── gpu/
│   │   │   │   │   ├── vulkan/
│   │   │   │   │   │   ├── SDL_gpu_vulkan.c.o
│   │   │   │   │   │   └── SDL_gpu_vulkan.c.o.d
│   │   │   │   │   ├── xr/
│   │   │   │   │   │   ├── SDL_gpu_openxr.c.o
│   │   │   │   │   │   ├── SDL_gpu_openxr.c.o.d
│   │   │   │   │   │   ├── SDL_openxrdyn.c.o
│   │   │   │   │   │   └── SDL_openxrdyn.c.o.d
│   │   │   │   │   ├── SDL_gpu.c.o
│   │   │   │   │   └── SDL_gpu.c.o.d
│   │   │   │   ├── haptic/
│   │   │   │   │   ├── hidapi/
│   │   │   │   │   │   ├── SDL_hidapihaptic_lg4ff.c.o
│   │   │   │   │   │   ├── SDL_hidapihaptic_lg4ff.c.o.d
│   │   │   │   │   │   ├── SDL_hidapihaptic.c.o
│   │   │   │   │   │   └── SDL_hidapihaptic.c.o.d
│   │   │   │   │   ├── linux/
│   │   │   │   │   │   ├── SDL_syshaptic.c.o
│   │   │   │   │   │   └── SDL_syshaptic.c.o.d
│   │   │   │   │   ├── SDL_haptic.c.o
│   │   │   │   │   └── SDL_haptic.c.o.d
│   │   │   │   ├── hidapi/
│   │   │   │   │   ├── SDL_hidapi.c.o
│   │   │   │   │   └── SDL_hidapi.c.o.d
│   │   │   │   ├── io/
│   │   │   │   │   ├── generic/
│   │   │   │   │   │   ├── SDL_asyncio_generic.c.o
│   │   │   │   │   │   └── SDL_asyncio_generic.c.o.d
│   │   │   │   │   ├── io_uring/
│   │   │   │   │   │   ├── SDL_asyncio_liburing.c.o
│   │   │   │   │   │   └── SDL_asyncio_liburing.c.o.d
│   │   │   │   │   ├── SDL_asyncio.c.o
│   │   │   │   │   ├── SDL_asyncio.c.o.d
│   │   │   │   │   ├── SDL_iostream.c.o
│   │   │   │   │   └── SDL_iostream.c.o.d
│   │   │   │   ├── joystick/
│   │   │   │   │   ├── hidapi/
│   │   │   │   │   │   ├── SDL_hidapi_8bitdo.c.o
│   │   │   │   │   │   ├── SDL_hidapi_8bitdo.c.o.d
│   │   │   │   │   │   ├── SDL_hidapi_combined.c.o
│   │   │   │   │   │   ├── SDL_hidapi_combined.c.o.d
│   │   │   │   │   │   ├── SDL_hidapi_flydigi.c.o
│   │   │   │   │   │   ├── SDL_hidapi_flydigi.c.o.d
│   │   │   │   │   │   ├── SDL_hidapi_gamecube.c.o
│   │   │   │   │   │   ├── SDL_hidapi_gamecube.c.o.d
│   │   │   │   │   │   ├── SDL_hidapi_gamesir.c.o
│   │   │   │   │   │   ├── SDL_hidapi_gamesir.c.o.d
│   │   │   │   │   │   ├── SDL_hidapi_gip.c.o
│   │   │   │   │   │   ├── SDL_hidapi_gip.c.o.d
│   │   │   │   │   │   ├── SDL_hidapi_lg4ff.c.o
│   │   │   │   │   │   ├── SDL_hidapi_lg4ff.c.o.d
│   │   │   │   │   │   ├── SDL_hidapi_luna.c.o
│   │   │   │   │   │   ├── SDL_hidapi_luna.c.o.d
│   │   │   │   │   │   ├── SDL_hidapi_ps3.c.o
│   │   │   │   │   │   ├── SDL_hidapi_ps3.c.o.d
│   │   │   │   │   │   ├── SDL_hidapi_ps4.c.o
│   │   │   │   │   │   ├── SDL_hidapi_ps4.c.o.d
│   │   │   │   │   │   ├── SDL_hidapi_ps5.c.o
│   │   │   │   │   │   ├── SDL_hidapi_ps5.c.o.d
│   │   │   │   │   │   ├── SDL_hidapi_rumble.c.o
│   │   │   │   │   │   ├── SDL_hidapi_rumble.c.o.d
│   │   │   │   │   │   ├── SDL_hidapi_shield.c.o
│   │   │   │   │   │   ├── SDL_hidapi_shield.c.o.d
│   │   │   │   │   │   ├── SDL_hidapi_sinput.c.o
│   │   │   │   │   │   ├── SDL_hidapi_sinput.c.o.d
│   │   │   │   │   │   ├── SDL_hidapi_stadia.c.o
│   │   │   │   │   │   ├── SDL_hidapi_stadia.c.o.d
│   │   │   │   │   │   ├── SDL_hidapi_steam_hori.c.o
│   │   │   │   │   │   ├── SDL_hidapi_steam_hori.c.o.d
│   │   │   │   │   │   ├── SDL_hidapi_steam_triton.c.o
│   │   │   │   │   │   ├── SDL_hidapi_steam_triton.c.o.d
│   │   │   │   │   │   ├── SDL_hidapi_steam.c.o
│   │   │   │   │   │   ├── SDL_hidapi_steam.c.o.d
│   │   │   │   │   │   ├── SDL_hidapi_steamdeck.c.o
│   │   │   │   │   │   ├── SDL_hidapi_steamdeck.c.o.d
│   │   │   │   │   │   ├── SDL_hidapi_switch.c.o
│   │   │   │   │   │   ├── SDL_hidapi_switch.c.o.d
│   │   │   │   │   │   ├── SDL_hidapi_switch2.c.o
│   │   │   │   │   │   ├── SDL_hidapi_switch2.c.o.d
│   │   │   │   │   │   ├── SDL_hidapi_wii.c.o
│   │   │   │   │   │   ├── SDL_hidapi_wii.c.o.d
│   │   │   │   │   │   ├── SDL_hidapi_xbox360.c.o
│   │   │   │   │   │   ├── SDL_hidapi_xbox360.c.o.d
│   │   │   │   │   │   ├── SDL_hidapi_xbox360bb.c.o
│   │   │   │   │   │   ├── SDL_hidapi_xbox360bb.c.o.d
│   │   │   │   │   │   ├── SDL_hidapi_xbox360w.c.o
│   │   │   │   │   │   ├── SDL_hidapi_xbox360w.c.o.d
│   │   │   │   │   │   ├── SDL_hidapi_xboxone.c.o
│   │   │   │   │   │   ├── SDL_hidapi_xboxone.c.o.d
│   │   │   │   │   │   ├── SDL_hidapi_zuiki.c.o
│   │   │   │   │   │   ├── SDL_hidapi_zuiki.c.o.d
│   │   │   │   │   │   ├── SDL_hidapijoystick.c.o
│   │   │   │   │   │   ├── SDL_hidapijoystick.c.o.d
│   │   │   │   │   │   ├── SDL_report_descriptor.c.o
│   │   │   │   │   │   └── SDL_report_descriptor.c.o.d
│   │   │   │   │   ├── linux/
│   │   │   │   │   │   ├── SDL_sysjoystick.c.o
│   │   │   │   │   │   └── SDL_sysjoystick.c.o.d
│   │   │   │   │   ├── virtual/
│   │   │   │   │   │   ├── SDL_virtualjoystick.c.o
│   │   │   │   │   │   └── SDL_virtualjoystick.c.o.d
│   │   │   │   │   ├── controller_type.c.o
│   │   │   │   │   ├── controller_type.c.o.d
│   │   │   │   │   ├── SDL_gamepad.c.o
│   │   │   │   │   ├── SDL_gamepad.c.o.d
│   │   │   │   │   ├── SDL_joystick.c.o
│   │   │   │   │   ├── SDL_joystick.c.o.d
│   │   │   │   │   ├── SDL_steam_virtual_gamepad.c.o
│   │   │   │   │   └── SDL_steam_virtual_gamepad.c.o.d
│   │   │   │   ├── loadso/
│   │   │   │   │   └── dlopen/
│   │   │   │   │       ├── SDL_sysloadso.c.o
│   │   │   │   │       └── SDL_sysloadso.c.o.d
│   │   │   │   ├── locale/
│   │   │   │   │   ├── unix/
│   │   │   │   │   │   ├── SDL_syslocale.c.o
│   │   │   │   │   │   └── SDL_syslocale.c.o.d
│   │   │   │   │   ├── SDL_locale.c.o
│   │   │   │   │   └── SDL_locale.c.o.d
│   │   │   │   ├── main/
│   │   │   │   │   ├── generic/
│   │   │   │   │   │   ├── SDL_sysmain_callbacks.c.o
│   │   │   │   │   │   └── SDL_sysmain_callbacks.c.o.d
│   │   │   │   │   ├── SDL_main_callbacks.c.o
│   │   │   │   │   ├── SDL_main_callbacks.c.o.d
│   │   │   │   │   ├── SDL_runapp.c.o
│   │   │   │   │   └── SDL_runapp.c.o.d
│   │   │   │   ├── misc/
│   │   │   │   │   ├── unix/
│   │   │   │   │   │   ├── SDL_sysurl.c.o
│   │   │   │   │   │   └── SDL_sysurl.c.o.d
│   │   │   │   │   ├── SDL_libusb.c.o
│   │   │   │   │   ├── SDL_libusb.c.o.d
│   │   │   │   │   ├── SDL_url.c.o
│   │   │   │   │   └── SDL_url.c.o.d
│   │   │   │   ├── notification/
│   │   │   │   │   ├── unix/
│   │   │   │   │   │   ├── SDL_dbusnotification.c.o
│   │   │   │   │   │   └── SDL_dbusnotification.c.o.d
│   │   │   │   │   ├── SDL_notification.c.o
│   │   │   │   │   └── SDL_notification.c.o.d
│   │   │   │   ├── power/
│   │   │   │   │   ├── linux/
│   │   │   │   │   │   ├── SDL_syspower.c.o
│   │   │   │   │   │   └── SDL_syspower.c.o.d
│   │   │   │   │   ├── SDL_power.c.o
│   │   │   │   │   └── SDL_power.c.o.d
│   │   │   │   ├── process/
│   │   │   │   │   ├── posix/
│   │   │   │   │   │   ├── SDL_posixprocess.c.o
│   │   │   │   │   │   └── SDL_posixprocess.c.o.d
│   │   │   │   │   ├── SDL_process.c.o
│   │   │   │   │   └── SDL_process.c.o.d
│   │   │   │   ├── render/
│   │   │   │   │   ├── direct3d/
│   │   │   │   │   │   ├── SDL_render_d3d.c.o
│   │   │   │   │   │   ├── SDL_render_d3d.c.o.d
│   │   │   │   │   │   ├── SDL_shaders_d3d.c.o
│   │   │   │   │   │   └── SDL_shaders_d3d.c.o.d
│   │   │   │   │   ├── direct3d11/
│   │   │   │   │   │   ├── SDL_render_d3d11.c.o
│   │   │   │   │   │   ├── SDL_render_d3d11.c.o.d
│   │   │   │   │   │   ├── SDL_shaders_d3d11.c.o
│   │   │   │   │   │   └── SDL_shaders_d3d11.c.o.d
│   │   │   │   │   ├── direct3d12/
│   │   │   │   │   │   ├── SDL_render_d3d12.c.o
│   │   │   │   │   │   ├── SDL_render_d3d12.c.o.d
│   │   │   │   │   │   ├── SDL_shaders_d3d12.c.o
│   │   │   │   │   │   └── SDL_shaders_d3d12.c.o.d
│   │   │   │   │   ├── gpu/
│   │   │   │   │   │   ├── SDL_pipeline_gpu.c.o
│   │   │   │   │   │   ├── SDL_pipeline_gpu.c.o.d
│   │   │   │   │   │   ├── SDL_render_gpu.c.o
│   │   │   │   │   │   ├── SDL_render_gpu.c.o.d
│   │   │   │   │   │   ├── SDL_shaders_gpu.c.o
│   │   │   │   │   │   └── SDL_shaders_gpu.c.o.d
│   │   │   │   │   ├── ngage/
│   │   │   │   │   │   ├── SDL_render_ngage.c.o
│   │   │   │   │   │   └── SDL_render_ngage.c.o.d
│   │   │   │   │   ├── opengl/
│   │   │   │   │   │   ├── SDL_render_gl.c.o
│   │   │   │   │   │   ├── SDL_render_gl.c.o.d
│   │   │   │   │   │   ├── SDL_shaders_gl.c.o
│   │   │   │   │   │   └── SDL_shaders_gl.c.o.d
│   │   │   │   │   ├── opengles/
│   │   │   │   │   │   ├── SDL_render_gles.c.o
│   │   │   │   │   │   └── SDL_render_gles.c.o.d
│   │   │   │   │   ├── opengles2/
│   │   │   │   │   │   ├── SDL_render_gles2.c.o
│   │   │   │   │   │   ├── SDL_render_gles2.c.o.d
│   │   │   │   │   │   ├── SDL_shaders_gles2.c.o
│   │   │   │   │   │   └── SDL_shaders_gles2.c.o.d
│   │   │   │   │   ├── ps2/
│   │   │   │   │   │   ├── SDL_render_ps2.c.o
│   │   │   │   │   │   └── SDL_render_ps2.c.o.d
│   │   │   │   │   ├── psp/
│   │   │   │   │   │   ├── SDL_render_psp.c.o
│   │   │   │   │   │   └── SDL_render_psp.c.o.d
│   │   │   │   │   ├── software/
│   │   │   │   │   │   ├── SDL_blendfillrect.c.o
│   │   │   │   │   │   ├── SDL_blendfillrect.c.o.d
│   │   │   │   │   │   ├── SDL_blendline.c.o
│   │   │   │   │   │   ├── SDL_blendline.c.o.d
│   │   │   │   │   │   ├── SDL_blendpoint.c.o
│   │   │   │   │   │   ├── SDL_blendpoint.c.o.d
│   │   │   │   │   │   ├── SDL_drawline.c.o
│   │   │   │   │   │   ├── SDL_drawline.c.o.d
│   │   │   │   │   │   ├── SDL_drawpoint.c.o
│   │   │   │   │   │   ├── SDL_drawpoint.c.o.d
│   │   │   │   │   │   ├── SDL_render_sw.c.o
│   │   │   │   │   │   ├── SDL_render_sw.c.o.d
│   │   │   │   │   │   ├── SDL_triangle.c.o
│   │   │   │   │   │   └── SDL_triangle.c.o.d
│   │   │   │   │   ├── vitagxm/
│   │   │   │   │   │   ├── SDL_render_vita_gxm_memory.c.o
│   │   │   │   │   │   ├── SDL_render_vita_gxm_memory.c.o.d
│   │   │   │   │   │   ├── SDL_render_vita_gxm_tools.c.o
│   │   │   │   │   │   ├── SDL_render_vita_gxm_tools.c.o.d
│   │   │   │   │   │   ├── SDL_render_vita_gxm.c.o
│   │   │   │   │   │   └── SDL_render_vita_gxm.c.o.d
│   │   │   │   │   ├── vulkan/
│   │   │   │   │   │   ├── SDL_render_vulkan.c.o
│   │   │   │   │   │   ├── SDL_render_vulkan.c.o.d
│   │   │   │   │   │   ├── SDL_shaders_vulkan.c.o
│   │   │   │   │   │   └── SDL_shaders_vulkan.c.o.d
│   │   │   │   │   ├── SDL_render_unsupported.c.o
│   │   │   │   │   ├── SDL_render_unsupported.c.o.d
│   │   │   │   │   ├── SDL_render.c.o
│   │   │   │   │   ├── SDL_render.c.o.d
│   │   │   │   │   ├── SDL_yuv_sw.c.o
│   │   │   │   │   └── SDL_yuv_sw.c.o.d
│   │   │   │   ├── sensor/
│   │   │   │   │   ├── dummy/
│   │   │   │   │   │   ├── SDL_dummysensor.c.o
│   │   │   │   │   │   └── SDL_dummysensor.c.o.d
│   │   │   │   │   ├── SDL_sensor.c.o
│   │   │   │   │   └── SDL_sensor.c.o.d
│   │   │   │   ├── stdlib/
│   │   │   │   │   ├── SDL_crc16.c.o
│   │   │   │   │   ├── SDL_crc16.c.o.d
│   │   │   │   │   ├── SDL_crc32.c.o
│   │   │   │   │   ├── SDL_crc32.c.o.d
│   │   │   │   │   ├── SDL_getenv.c.o
│   │   │   │   │   ├── SDL_getenv.c.o.d
│   │   │   │   │   ├── SDL_iconv.c.o
│   │   │   │   │   ├── SDL_iconv.c.o.d
│   │   │   │   │   ├── SDL_malloc.c.o
│   │   │   │   │   ├── SDL_malloc.c.o.d
│   │   │   │   │   ├── SDL_memcpy.c.o
│   │   │   │   │   ├── SDL_memcpy.c.o.d
│   │   │   │   │   ├── SDL_memmove.c.o
│   │   │   │   │   ├── SDL_memmove.c.o.d
│   │   │   │   │   ├── SDL_memset.c.o
│   │   │   │   │   ├── SDL_memset.c.o.d
│   │   │   │   │   ├── SDL_mslibc.c.o
│   │   │   │   │   ├── SDL_mslibc.c.o.d
│   │   │   │   │   ├── SDL_murmur3.c.o
│   │   │   │   │   ├── SDL_murmur3.c.o.d
│   │   │   │   │   ├── SDL_qsort.c.o
│   │   │   │   │   ├── SDL_qsort.c.o.d
│   │   │   │   │   ├── SDL_random.c.o
│   │   │   │   │   ├── SDL_random.c.o.d
│   │   │   │   │   ├── SDL_stdlib.c.o
│   │   │   │   │   ├── SDL_stdlib.c.o.d
│   │   │   │   │   ├── SDL_string.c.o
│   │   │   │   │   ├── SDL_string.c.o.d
│   │   │   │   │   ├── SDL_strtokr.c.o
│   │   │   │   │   └── SDL_strtokr.c.o.d
│   │   │   │   ├── storage/
│   │   │   │   │   ├── generic/
│   │   │   │   │   │   ├── SDL_genericstorage.c.o
│   │   │   │   │   │   └── SDL_genericstorage.c.o.d
│   │   │   │   │   ├── steam/
│   │   │   │   │   │   ├── SDL_steamstorage.c.o
│   │   │   │   │   │   └── SDL_steamstorage.c.o.d
│   │   │   │   │   ├── SDL_storage.c.o
│   │   │   │   │   └── SDL_storage.c.o.d
│   │   │   │   ├── thread/
│   │   │   │   │   ├── pthread/
│   │   │   │   │   │   ├── SDL_syscond.c.o
│   │   │   │   │   │   ├── SDL_syscond.c.o.d
│   │   │   │   │   │   ├── SDL_sysmutex.c.o
│   │   │   │   │   │   ├── SDL_sysmutex.c.o.d
│   │   │   │   │   │   ├── SDL_sysrwlock.c.o
│   │   │   │   │   │   ├── SDL_sysrwlock.c.o.d
│   │   │   │   │   │   ├── SDL_syssem.c.o
│   │   │   │   │   │   ├── SDL_syssem.c.o.d
│   │   │   │   │   │   ├── SDL_systhread.c.o
│   │   │   │   │   │   ├── SDL_systhread.c.o.d
│   │   │   │   │   │   ├── SDL_systls.c.o
│   │   │   │   │   │   └── SDL_systls.c.o.d
│   │   │   │   │   ├── SDL_thread.c.o
│   │   │   │   │   └── SDL_thread.c.o.d
│   │   │   │   ├── time/
│   │   │   │   │   ├── unix/
│   │   │   │   │   │   ├── SDL_systime.c.o
│   │   │   │   │   │   └── SDL_systime.c.o.d
│   │   │   │   │   ├── SDL_time.c.o
│   │   │   │   │   └── SDL_time.c.o.d
│   │   │   │   ├── timer/
│   │   │   │   │   ├── unix/
│   │   │   │   │   │   ├── SDL_systimer.c.o
│   │   │   │   │   │   └── SDL_systimer.c.o.d
│   │   │   │   │   ├── SDL_timer.c.o
│   │   │   │   │   └── SDL_timer.c.o.d
│   │   │   │   ├── tray/
│   │   │   │   │   ├── unix/
│   │   │   │   │   │   ├── SDL_dbustray.c.o
│   │   │   │   │   │   ├── SDL_dbustray.c.o.d
│   │   │   │   │   │   ├── SDL_tray.c.o
│   │   │   │   │   │   └── SDL_tray.c.o.d
│   │   │   │   │   ├── SDL_tray_utils.c.o
│   │   │   │   │   └── SDL_tray_utils.c.o.d
│   │   │   │   ├── video/
│   │   │   │   │   ├── dummy/
│   │   │   │   │   │   ├── SDL_nullevents.c.o
│   │   │   │   │   │   ├── SDL_nullevents.c.o.d
│   │   │   │   │   │   ├── SDL_nullframebuffer.c.o
│   │   │   │   │   │   ├── SDL_nullframebuffer.c.o.d
│   │   │   │   │   │   ├── SDL_nullvideo.c.o
│   │   │   │   │   │   └── SDL_nullvideo.c.o.d
│   │   │   │   │   ├── kmsdrm/
│   │   │   │   │   │   ├── SDL_kmsdrmdyn.c.o
│   │   │   │   │   │   ├── SDL_kmsdrmdyn.c.o.d
│   │   │   │   │   │   ├── SDL_kmsdrmevents.c.o
│   │   │   │   │   │   ├── SDL_kmsdrmevents.c.o.d
│   │   │   │   │   │   ├── SDL_kmsdrmmouse.c.o
│   │   │   │   │   │   ├── SDL_kmsdrmmouse.c.o.d
│   │   │   │   │   │   ├── SDL_kmsdrmopengles.c.o
│   │   │   │   │   │   ├── SDL_kmsdrmopengles.c.o.d
│   │   │   │   │   │   ├── SDL_kmsdrmvideo.c.o
│   │   │   │   │   │   ├── SDL_kmsdrmvideo.c.o.d
│   │   │   │   │   │   ├── SDL_kmsdrmvulkan.c.o
│   │   │   │   │   │   └── SDL_kmsdrmvulkan.c.o.d
│   │   │   │   │   ├── offscreen/
│   │   │   │   │   │   ├── SDL_offscreenevents.c.o
│   │   │   │   │   │   ├── SDL_offscreenevents.c.o.d
│   │   │   │   │   │   ├── SDL_offscreenframebuffer.c.o
│   │   │   │   │   │   ├── SDL_offscreenframebuffer.c.o.d
│   │   │   │   │   │   ├── SDL_offscreenopengles.c.o
│   │   │   │   │   │   ├── SDL_offscreenopengles.c.o.d
│   │   │   │   │   │   ├── SDL_offscreenvideo.c.o
│   │   │   │   │   │   ├── SDL_offscreenvideo.c.o.d
│   │   │   │   │   │   ├── SDL_offscreenvulkan.c.o
│   │   │   │   │   │   ├── SDL_offscreenvulkan.c.o.d
│   │   │   │   │   │   ├── SDL_offscreenwindow.c.o
│   │   │   │   │   │   └── SDL_offscreenwindow.c.o.d
│   │   │   │   │   ├── wayland/
│   │   │   │   │   │   ├── SDL_waylandclipboard.c.o
│   │   │   │   │   │   ├── SDL_waylandclipboard.c.o.d
│   │   │   │   │   │   ├── SDL_waylandcolor.c.o
│   │   │   │   │   │   ├── SDL_waylandcolor.c.o.d
│   │   │   │   │   │   ├── SDL_waylanddatamanager.c.o
│   │   │   │   │   │   ├── SDL_waylanddatamanager.c.o.d
│   │   │   │   │   │   ├── SDL_waylanddyn.c.o
│   │   │   │   │   │   ├── SDL_waylanddyn.c.o.d
│   │   │   │   │   │   ├── SDL_waylandevents.c.o
│   │   │   │   │   │   ├── SDL_waylandevents.c.o.d
│   │   │   │   │   │   ├── SDL_waylandeventthread.c.o
│   │   │   │   │   │   ├── SDL_waylandeventthread.c.o.d
│   │   │   │   │   │   ├── SDL_waylandkeyboard.c.o
│   │   │   │   │   │   ├── SDL_waylandkeyboard.c.o.d
│   │   │   │   │   │   ├── SDL_waylandmessagebox.c.o
│   │   │   │   │   │   ├── SDL_waylandmessagebox.c.o.d
│   │   │   │   │   │   ├── SDL_waylandmouse.c.o
│   │   │   │   │   │   ├── SDL_waylandmouse.c.o.d
│   │   │   │   │   │   ├── SDL_waylandopengles.c.o
│   │   │   │   │   │   ├── SDL_waylandopengles.c.o.d
│   │   │   │   │   │   ├── SDL_waylandshmbuffer.c.o
│   │   │   │   │   │   ├── SDL_waylandshmbuffer.c.o.d
│   │   │   │   │   │   ├── SDL_waylandutil.c.o
│   │   │   │   │   │   ├── SDL_waylandutil.c.o.d
│   │   │   │   │   │   ├── SDL_waylandvideo.c.o
│   │   │   │   │   │   ├── SDL_waylandvideo.c.o.d
│   │   │   │   │   │   ├── SDL_waylandvulkan.c.o
│   │   │   │   │   │   ├── SDL_waylandvulkan.c.o.d
│   │   │   │   │   │   ├── SDL_waylandwindow.c.o
│   │   │   │   │   │   └── SDL_waylandwindow.c.o.d
│   │   │   │   │   ├── x11/
│   │   │   │   │   │   ├── edid-parse.c.o
│   │   │   │   │   │   ├── edid-parse.c.o.d
│   │   │   │   │   │   ├── SDL_x11clipboard.c.o
│   │   │   │   │   │   ├── SDL_x11clipboard.c.o.d
│   │   │   │   │   │   ├── SDL_x11dyn.c.o
│   │   │   │   │   │   ├── SDL_x11dyn.c.o.d
│   │   │   │   │   │   ├── SDL_x11events.c.o
│   │   │   │   │   │   ├── SDL_x11events.c.o.d
│   │   │   │   │   │   ├── SDL_x11framebuffer.c.o
│   │   │   │   │   │   ├── SDL_x11framebuffer.c.o.d
│   │   │   │   │   │   ├── SDL_x11keyboard.c.o
│   │   │   │   │   │   ├── SDL_x11keyboard.c.o.d
│   │   │   │   │   │   ├── SDL_x11messagebox.c.o
│   │   │   │   │   │   ├── SDL_x11messagebox.c.o.d
│   │   │   │   │   │   ├── SDL_x11modes.c.o
│   │   │   │   │   │   ├── SDL_x11modes.c.o.d
│   │   │   │   │   │   ├── SDL_x11mouse.c.o
│   │   │   │   │   │   ├── SDL_x11mouse.c.o.d
│   │   │   │   │   │   ├── SDL_x11opengl.c.o
│   │   │   │   │   │   ├── SDL_x11opengl.c.o.d
│   │   │   │   │   │   ├── SDL_x11opengles.c.o
│   │   │   │   │   │   ├── SDL_x11opengles.c.o.d
│   │   │   │   │   │   ├── SDL_x11pen.c.o
│   │   │   │   │   │   ├── SDL_x11pen.c.o.d
│   │   │   │   │   │   ├── SDL_x11settings.c.o
│   │   │   │   │   │   ├── SDL_x11settings.c.o.d
│   │   │   │   │   │   ├── SDL_x11shape.c.o
│   │   │   │   │   │   ├── SDL_x11shape.c.o.d
│   │   │   │   │   │   ├── SDL_x11toolkit.c.o
│   │   │   │   │   │   ├── SDL_x11toolkit.c.o.d
│   │   │   │   │   │   ├── SDL_x11touch.c.o
│   │   │   │   │   │   ├── SDL_x11touch.c.o.d
│   │   │   │   │   │   ├── SDL_x11video.c.o
│   │   │   │   │   │   ├── SDL_x11video.c.o.d
│   │   │   │   │   │   ├── SDL_x11vulkan.c.o
│   │   │   │   │   │   ├── SDL_x11vulkan.c.o.d
│   │   │   │   │   │   ├── SDL_x11window.c.o
│   │   │   │   │   │   ├── SDL_x11window.c.o.d
│   │   │   │   │   │   ├── SDL_x11xfixes.c.o
│   │   │   │   │   │   ├── SDL_x11xfixes.c.o.d
│   │   │   │   │   │   ├── SDL_x11xinput2.c.o
│   │   │   │   │   │   ├── SDL_x11xinput2.c.o.d
│   │   │   │   │   │   ├── SDL_x11xsync.c.o
│   │   │   │   │   │   ├── SDL_x11xsync.c.o.d
│   │   │   │   │   │   ├── SDL_x11xtest.c.o
│   │   │   │   │   │   ├── SDL_x11xtest.c.o.d
│   │   │   │   │   │   ├── xsettings-client.c.o
│   │   │   │   │   │   └── xsettings-client.c.o.d
│   │   │   │   │   ├── yuv2rgb/
│   │   │   │   │   │   ├── yuv_rgb_lsx.c.o
│   │   │   │   │   │   ├── yuv_rgb_lsx.c.o.d
│   │   │   │   │   │   ├── yuv_rgb_sse.c.o
│   │   │   │   │   │   ├── yuv_rgb_sse.c.o.d
│   │   │   │   │   │   ├── yuv_rgb_std.c.o
│   │   │   │   │   │   └── yuv_rgb_std.c.o.d
│   │   │   │   │   ├── SDL_blit_0.c.o
│   │   │   │   │   ├── SDL_blit_0.c.o.d
│   │   │   │   │   ├── SDL_blit_1.c.o
│   │   │   │   │   ├── SDL_blit_1.c.o.d
│   │   │   │   │   ├── SDL_blit_A.c.o
│   │   │   │   │   ├── SDL_blit_A.c.o.d
│   │   │   │   │   ├── SDL_blit_auto.c.o
│   │   │   │   │   ├── SDL_blit_auto.c.o.d
│   │   │   │   │   ├── SDL_blit_copy.c.o
│   │   │   │   │   ├── SDL_blit_copy.c.o.d
│   │   │   │   │   ├── SDL_blit_N.c.o
│   │   │   │   │   ├── SDL_blit_N.c.o.d
│   │   │   │   │   ├── SDL_blit_slow.c.o
│   │   │   │   │   ├── SDL_blit_slow.c.o.d
│   │   │   │   │   ├── SDL_blit.c.o
│   │   │   │   │   ├── SDL_blit.c.o.d
│   │   │   │   │   ├── SDL_bmp.c.o
│   │   │   │   │   ├── SDL_bmp.c.o.d
│   │   │   │   │   ├── SDL_clipboard.c.o
│   │   │   │   │   ├── SDL_clipboard.c.o.d
│   │   │   │   │   ├── SDL_egl.c.o
│   │   │   │   │   ├── SDL_egl.c.o.d
│   │   │   │   │   ├── SDL_fillrect.c.o
│   │   │   │   │   ├── SDL_fillrect.c.o.d
│   │   │   │   │   ├── SDL_pixels.c.o
│   │   │   │   │   ├── SDL_pixels.c.o.d
│   │   │   │   │   ├── SDL_rect.c.o
│   │   │   │   │   ├── SDL_rect.c.o.d
│   │   │   │   │   ├── SDL_RLEaccel.c.o
│   │   │   │   │   ├── SDL_RLEaccel.c.o.d
│   │   │   │   │   ├── SDL_rotate.c.o
│   │   │   │   │   ├── SDL_rotate.c.o.d
│   │   │   │   │   ├── SDL_stb.c.o
│   │   │   │   │   ├── SDL_stb.c.o.d
│   │   │   │   │   ├── SDL_stretch.c.o
│   │   │   │   │   ├── SDL_stretch.c.o.d
│   │   │   │   │   ├── SDL_surface.c.o
│   │   │   │   │   ├── SDL_surface.c.o.d
│   │   │   │   │   ├── SDL_video_unsupported.c.o
│   │   │   │   │   ├── SDL_video_unsupported.c.o.d
│   │   │   │   │   ├── SDL_video.c.o
│   │   │   │   │   ├── SDL_video.c.o.d
│   │   │   │   │   ├── SDL_vulkan_utils.c.o
│   │   │   │   │   ├── SDL_vulkan_utils.c.o.d
│   │   │   │   │   ├── SDL_yuv.c.o
│   │   │   │   │   └── SDL_yuv.c.o.d
│   │   │   │   ├── SDL_assert.c.o
│   │   │   │   ├── SDL_assert.c.o.d
│   │   │   │   ├── SDL_error.c.o
│   │   │   │   ├── SDL_error.c.o.d
│   │   │   │   ├── SDL_guid.c.o
│   │   │   │   ├── SDL_guid.c.o.d
│   │   │   │   ├── SDL_hashtable.c.o
│   │   │   │   ├── SDL_hashtable.c.o.d
│   │   │   │   ├── SDL_hints.c.o
│   │   │   │   ├── SDL_hints.c.o.d
│   │   │   │   ├── SDL_list.c.o
│   │   │   │   ├── SDL_list.c.o.d
│   │   │   │   ├── SDL_log.c.o
│   │   │   │   ├── SDL_log.c.o.d
│   │   │   │   ├── SDL_properties.c.o
│   │   │   │   ├── SDL_properties.c.o.d
│   │   │   │   ├── SDL_utils.c.o
│   │   │   │   ├── SDL_utils.c.o.d
│   │   │   │   ├── SDL.c.o
│   │   │   │   └── SDL.c.o.d
│   │   │   ├── wayland-generated-protocols/
│   │   │   │   ├── alpha-modifier-v1-protocol.c.o
│   │   │   │   ├── alpha-modifier-v1-protocol.c.o.d
│   │   │   │   ├── color-management-v1-protocol.c.o
│   │   │   │   ├── color-management-v1-protocol.c.o.d
│   │   │   │   ├── cursor-shape-v1-protocol.c.o
│   │   │   │   ├── cursor-shape-v1-protocol.c.o.d
│   │   │   │   ├── fractional-scale-v1-protocol.c.o
│   │   │   │   ├── fractional-scale-v1-protocol.c.o.d
│   │   │   │   ├── frog-color-management-v1-protocol.c.o
│   │   │   │   ├── frog-color-management-v1-protocol.c.o.d
│   │   │   │   ├── idle-inhibit-unstable-v1-protocol.c.o
│   │   │   │   ├── idle-inhibit-unstable-v1-protocol.c.o.d
│   │   │   │   ├── input-timestamps-unstable-v1-protocol.c.o
│   │   │   │   ├── input-timestamps-unstable-v1-protocol.c.o.d
│   │   │   │   ├── keyboard-shortcuts-inhibit-unstable-v1-protocol.c.o
│   │   │   │   ├── keyboard-shortcuts-inhibit-unstable-v1-protocol.c.o.d
│   │   │   │   ├── pointer-constraints-unstable-v1-protocol.c.o
│   │   │   │   ├── pointer-constraints-unstable-v1-protocol.c.o.d
│   │   │   │   ├── pointer-gestures-unstable-v1-protocol.c.o
│   │   │   │   ├── pointer-gestures-unstable-v1-protocol.c.o.d
│   │   │   │   ├── pointer-warp-v1-protocol.c.o
│   │   │   │   ├── pointer-warp-v1-protocol.c.o.d
│   │   │   │   ├── primary-selection-unstable-v1-protocol.c.o
│   │   │   │   ├── primary-selection-unstable-v1-protocol.c.o.d
│   │   │   │   ├── relative-pointer-unstable-v1-protocol.c.o
│   │   │   │   ├── relative-pointer-unstable-v1-protocol.c.o.d
│   │   │   │   ├── single-pixel-buffer-v1-protocol.c.o
│   │   │   │   ├── single-pixel-buffer-v1-protocol.c.o.d
│   │   │   │   ├── tablet-v2-protocol.c.o
│   │   │   │   ├── tablet-v2-protocol.c.o.d
│   │   │   │   ├── text-input-unstable-v3-protocol.c.o
│   │   │   │   ├── text-input-unstable-v3-protocol.c.o.d
│   │   │   │   ├── viewporter-protocol.c.o
│   │   │   │   ├── viewporter-protocol.c.o.d
│   │   │   │   ├── wayland-protocol.c.o
│   │   │   │   ├── wayland-protocol.c.o.d
│   │   │   │   ├── xdg-activation-v1-protocol.c.o
│   │   │   │   ├── xdg-activation-v1-protocol.c.o.d
│   │   │   │   ├── xdg-decoration-unstable-v1-protocol.c.o
│   │   │   │   ├── xdg-decoration-unstable-v1-protocol.c.o.d
│   │   │   │   ├── xdg-dialog-v1-protocol.c.o
│   │   │   │   ├── xdg-dialog-v1-protocol.c.o.d
│   │   │   │   ├── xdg-foreign-unstable-v2-protocol.c.o
│   │   │   │   ├── xdg-foreign-unstable-v2-protocol.c.o.d
│   │   │   │   ├── xdg-output-unstable-v1-protocol.c.o
│   │   │   │   ├── xdg-output-unstable-v1-protocol.c.o.d
│   │   │   │   ├── xdg-session-management-v1-protocol.c.o
│   │   │   │   ├── xdg-session-management-v1-protocol.c.o.d
│   │   │   │   ├── xdg-shell-protocol.c.o
│   │   │   │   ├── xdg-shell-protocol.c.o.d
│   │   │   │   ├── xdg-toplevel-icon-v1-protocol.c.o
│   │   │   │   ├── xdg-toplevel-icon-v1-protocol.c.o.d
│   │   │   │   ├── xdg-toplevel-tag-v1-protocol.c.o
│   │   │   │   └── xdg-toplevel-tag-v1-protocol.c.o.d
│   │   │   ├── build.make
│   │   │   ├── cmake_clean.cmake
│   │   │   ├── cmake_pch.h
│   │   │   ├── cmake_pch.h.c
│   │   │   ├── cmake_pch.h.gch
│   │   │   ├── cmake_pch.h.gch.d
│   │   │   ├── compiler_depend.internal
│   │   │   ├── compiler_depend.make
│   │   │   ├── compiler_depend.ts
│   │   │   ├── depend.make
│   │   │   ├── DependInfo.cmake
│   │   │   ├── flags.make
│   │   │   ├── link.txt
│   │   │   └── progress.make
│   │   ├── uninstall.dir/
│   │   │   ├── build.make
│   │   │   ├── cmake_clean.cmake
│   │   │   ├── compiler_depend.make
│   │   │   ├── compiler_depend.ts
│   │   │   ├── DependInfo.cmake
│   │   │   └── progress.make
│   │   ├── cmake.check_cache
│   │   ├── cmake.verify_globs
│   │   ├── CMakeConfigureLog.yaml
│   │   ├── CMakeDirectoryInformation.cmake
│   │   ├── CMakeRuleHashes.txt
│   │   ├── dummy.sym
│   │   ├── Makefile.cmake
│   │   ├── Makefile2
│   │   ├── progress.marks
│   │   ├── SDL_build_config.h.intermediate
│   │   ├── TargetDirectories.txt
│   │   └── VerifyGlobs.cmake
│   ├── include-config-relwithdebinfo/
│   │   └── build_config/
│   │       └── SDL_build_config.h
│   ├── include-revision/
│   │   └── SDL3/
│   │       └── SDL_revision.h
│   ├── test/
│   │   ├── CMakeFiles/
│   │   │   ├── CheckCXX/
│   │   │   │   ├── CMakeFiles/
│   │   │   │   │   ├── 3.28.3/
│   │   │   │   │   │   ├── CompilerIdCXX/
│   │   │   │   │   │   │   ├── tmp/
│   │   │   │   │   │   │   ├── a.out
│   │   │   │   │   │   │   └── CMakeCXXCompilerId.cpp
│   │   │   │   │   │   ├── CMakeCXXCompiler.cmake
│   │   │   │   │   │   ├── CMakeDetermineCompilerABI_CXX.bin
│   │   │   │   │   │   └── CMakeSystem.cmake
│   │   │   │   │   ├── CMakeScratch/
│   │   │   │   │   ├── pkgRedirects/
│   │   │   │   │   ├── cmake.check_cache
│   │   │   │   │   ├── CMakeConfigureLog.yaml
│   │   │   │   │   ├── CMakeDirectoryInformation.cmake
│   │   │   │   │   ├── Makefile.cmake
│   │   │   │   │   ├── Makefile2
│   │   │   │   │   ├── progress.marks
│   │   │   │   │   └── TargetDirectories.txt
│   │   │   │   ├── cmake_install.cmake
│   │   │   │   ├── CMakeCache.txt
│   │   │   │   ├── CMakeLists.txt
│   │   │   │   ├── Makefile
│   │   │   │   └── result.cmake
│   │   │   ├── checkkeys.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   └── progress.make
│   │   │   ├── childprocess.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   └── progress.make
│   │   │   ├── copy-sdl-test-resources.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   └── progress.make
│   │   │   ├── generate-gamepad_image_headers.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   └── progress.make
│   │   │   ├── generate-glass_png_header.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   └── progress.make
│   │   │   ├── generate-icon_png_header.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   └── progress.make
│   │   │   ├── loopwave.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   └── progress.make
│   │   │   ├── sdltests_utils.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   └── progress.make
│   │   │   ├── testasyncio.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   └── progress.make
│   │   │   ├── testatomic.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   └── progress.make
│   │   │   ├── testaudio.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   └── progress.make
│   │   │   ├── testaudiohotplug.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   └── progress.make
│   │   │   ├── testaudioinfo.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   └── progress.make
│   │   │   ├── testaudiorecording.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   └── progress.make
│   │   │   ├── testaudiostreamdynamicresample.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   └── progress.make
│   │   │   ├── testautomation.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   └── progress.make
│   │   │   ├── testbounds.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   └── progress.make
│   │   │   ├── testcamera.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   └── progress.make
│   │   │   ├── testclipboard.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   └── progress.make
│   │   │   ├── testcolorspace.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   └── progress.make
│   │   │   ├── testcontroller.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   └── progress.make
│   │   │   ├── testcustomcursor.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   └── progress.make
│   │   │   ├── testdescriptor.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   └── progress.make
│   │   │   ├── testdialog.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   └── progress.make
│   │   │   ├── testdisplayinfo.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   └── progress.make
│   │   │   ├── testdlopennote.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   └── progress.make
│   │   │   ├── testdraw.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   └── progress.make
│   │   │   ├── testdrawchessboard.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   └── progress.make
│   │   │   ├── testdropfile.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   └── progress.make
│   │   │   ├── testdynaudioreopen.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   └── progress.make
│   │   │   ├── testerror.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   └── progress.make
│   │   │   ├── testevdev.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   └── progress.make
│   │   │   ├── testfile.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   └── progress.make
│   │   │   ├── testfilesystem.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   └── progress.make
│   │   │   ├── testgeometry.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   └── progress.make
│   │   │   ├── testgl.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   └── progress.make
│   │   │   ├── testgles.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   └── progress.make
│   │   │   ├── testgles2.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   └── progress.make
│   │   │   ├── testgpu_simple_clear.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   └── progress.make
│   │   │   ├── testgpu_spinning_cube_xr.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   └── progress.make
│   │   │   ├── testgpu_spinning_cube.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   └── progress.make
│   │   │   ├── testgpurender_effects.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   └── progress.make
│   │   │   ├── testgpurender_msdf.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   └── progress.make
│   │   │   ├── testhaptic.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   └── progress.make
│   │   │   ├── testhittesting.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   └── progress.make
│   │   │   ├── testhotplug.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   └── progress.make
│   │   │   ├── testiconv.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   └── progress.make
│   │   │   ├── testime.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   └── progress.make
│   │   │   ├── testintersections.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   └── progress.make
│   │   │   ├── testkeys.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   └── progress.make
│   │   │   ├── testloadso.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   └── progress.make
│   │   │   ├── testlocale.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   └── progress.make
│   │   │   ├── testlock.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   └── progress.make
│   │   │   ├── testmanymouse.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   └── progress.make
│   │   │   ├── testmessage.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   └── progress.make
│   │   │   ├── testmodal.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   └── progress.make
│   │   │   ├── testmouse.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   └── progress.make
│   │   │   ├── testmultiaudio.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   └── progress.make
│   │   │   ├── testnative.dir/
│   │   │   │   ├── __/
│   │   │   │   │   └── wayland-generated-protocols/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   └── progress.make
│   │   │   ├── testnotification.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   └── progress.make
│   │   │   ├── testoffscreen.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   └── progress.make
│   │   │   ├── testoverlay.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   └── progress.make
│   │   │   ├── testpalette.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   └── progress.make
│   │   │   ├── testpen.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   └── progress.make
│   │   │   ├── testplatform.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   └── progress.make
│   │   │   ├── testpopup.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   └── progress.make
│   │   │   ├── testpower.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   └── progress.make
│   │   │   ├── testprocess.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   └── progress.make
│   │   │   ├── testqsort.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   └── progress.make
│   │   │   ├── testrelative.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   └── progress.make
│   │   │   ├── testrendercopyex.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   └── progress.make
│   │   │   ├── testrendertarget.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   └── progress.make
│   │   │   ├── testresample.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   └── progress.make
│   │   │   ├── testrotate.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   └── progress.make
│   │   │   ├── testrumble.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   └── progress.make
│   │   │   ├── testrwlock.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   └── progress.make
│   │   │   ├── testscale.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   └── progress.make
│   │   │   ├── testsem.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   └── progress.make
│   │   │   ├── testsensor.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   └── progress.make
│   │   │   ├── testshader.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   └── progress.make
│   │   │   ├── testshape.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   └── progress.make
│   │   │   ├── testsoftwaretransparent.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   └── progress.make
│   │   │   ├── testsprite.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   └── progress.make
│   │   │   ├── testspritecxx.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   └── progress.make
│   │   │   ├── testspriteminimal.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   └── progress.make
│   │   │   ├── testspritesurface.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   └── progress.make
│   │   │   ├── testsurround.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   └── progress.make
│   │   │   ├── testsymbols.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   └── progress.make
│   │   │   ├── testthread.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   └── progress.make
│   │   │   ├── testtime.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   └── progress.make
│   │   │   ├── testtimer.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   └── progress.make
│   │   │   ├── testtray.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   └── progress.make
│   │   │   ├── testurl.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   └── progress.make
│   │   │   ├── testver.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   └── progress.make
│   │   │   ├── testviewport.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   └── progress.make
│   │   │   ├── testvulkan.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   └── progress.make
│   │   │   ├── testwaylandcustom.dir/
│   │   │   │   ├── __/
│   │   │   │   │   └── wayland-generated-protocols/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   └── progress.make
│   │   │   ├── testwm.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   └── progress.make
│   │   │   ├── testx11windowdestroy.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.internal
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   ├── progress.make
│   │   │   │   ├── testx11windowdestroy.c.o
│   │   │   │   └── testx11windowdestroy.c.o.d
│   │   │   ├── testyuv.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   └── progress.make
│   │   │   ├── torturethread.dir/
│   │   │   │   ├── build.make
│   │   │   │   ├── cmake_clean.cmake
│   │   │   │   ├── compiler_depend.make
│   │   │   │   ├── compiler_depend.ts
│   │   │   │   ├── depend.make
│   │   │   │   ├── DependInfo.cmake
│   │   │   │   ├── flags.make
│   │   │   │   ├── link.txt
│   │   │   │   └── progress.make
│   │   │   ├── CMakeDirectoryInformation.cmake
│   │   │   └── progress.marks
│   │   ├── cmake_install.cmake
│   │   ├── CTestTestfile.cmake
│   │   ├── Makefile
│   │   └── testx11windowdestroy
│   ├── Testing/
│   │   └── Temporary/
│   │       ├── CTestCostData.txt
│   │       ├── LastTest.log
│   │       └── LastTestsFailed.log
│   ├── wayland-generated-protocols/
│   │   ├── alpha-modifier-v1-client-protocol.h
│   │   ├── alpha-modifier-v1-protocol.c
│   │   ├── color-management-v1-client-protocol.h
│   │   ├── color-management-v1-protocol.c
│   │   ├── cursor-shape-v1-client-protocol.h
│   │   ├── cursor-shape-v1-protocol.c
│   │   ├── fractional-scale-v1-client-protocol.h
│   │   ├── fractional-scale-v1-protocol.c
│   │   ├── frog-color-management-v1-client-protocol.h
│   │   ├── frog-color-management-v1-protocol.c
│   │   ├── idle-inhibit-unstable-v1-client-protocol.h
│   │   ├── idle-inhibit-unstable-v1-protocol.c
│   │   ├── input-timestamps-unstable-v1-client-protocol.h
│   │   ├── input-timestamps-unstable-v1-protocol.c
│   │   ├── keyboard-shortcuts-inhibit-unstable-v1-client-protocol.h
│   │   ├── keyboard-shortcuts-inhibit-unstable-v1-protocol.c
│   │   ├── pointer-constraints-unstable-v1-client-protocol.h
│   │   ├── pointer-constraints-unstable-v1-protocol.c
│   │   ├── pointer-gestures-unstable-v1-client-protocol.h
│   │   ├── pointer-gestures-unstable-v1-protocol.c
│   │   ├── pointer-warp-v1-client-protocol.h
│   │   ├── pointer-warp-v1-protocol.c
│   │   ├── primary-selection-unstable-v1-client-protocol.h
│   │   ├── primary-selection-unstable-v1-protocol.c
│   │   ├── relative-pointer-unstable-v1-client-protocol.h
│   │   ├── relative-pointer-unstable-v1-protocol.c
│   │   ├── single-pixel-buffer-v1-client-protocol.h
│   │   ├── single-pixel-buffer-v1-protocol.c
│   │   ├── tablet-v2-client-protocol.h
│   │   ├── tablet-v2-protocol.c
│   │   ├── text-input-unstable-v3-client-protocol.h
│   │   ├── text-input-unstable-v3-protocol.c
│   │   ├── viewporter-client-protocol.h
│   │   ├── viewporter-protocol.c
│   │   ├── wayland-client-protocol.h
│   │   ├── wayland-protocol.c
│   │   ├── xdg-activation-v1-client-protocol.h
│   │   ├── xdg-activation-v1-protocol.c
│   │   ├── xdg-decoration-unstable-v1-client-protocol.h
│   │   ├── xdg-decoration-unstable-v1-protocol.c
│   │   ├── xdg-dialog-v1-client-protocol.h
│   │   ├── xdg-dialog-v1-protocol.c
│   │   ├── xdg-foreign-unstable-v2-client-protocol.h
│   │   ├── xdg-foreign-unstable-v2-protocol.c
│   │   ├── xdg-output-unstable-v1-client-protocol.h
│   │   ├── xdg-output-unstable-v1-protocol.c
│   │   ├── xdg-session-management-v1-client-protocol.h
│   │   ├── xdg-session-management-v1-protocol.c
│   │   ├── xdg-shell-client-protocol.h
│   │   ├── xdg-shell-protocol.c
│   │   ├── xdg-toplevel-icon-v1-client-protocol.h
│   │   ├── xdg-toplevel-icon-v1-protocol.c
│   │   ├── xdg-toplevel-tag-v1-client-protocol.h
│   │   └── xdg-toplevel-tag-v1-protocol.c
│   ├── cmake_install.cmake
│   ├── cmake_uninstall.cmake
│   ├── CMakeCache.txt
│   ├── CPackConfig.cmake
│   ├── CPackProjectConfig.cmake
│   ├── CPackSourceConfig.cmake
│   ├── CTestTestfile.cmake
│   ├── libSDL_uclibc.a
│   ├── libSDL3_test.a
│   ├── libSDL3.so.0.5.0
│   ├── Makefile
│   ├── sdl3.pc
│   ├── SDL3Config.cmake
│   ├── SDL3ConfigVersion.cmake
│   ├── SDL3headersTargets.cmake
│   ├── SDL3sharedTargets.cmake
│   └── SDL3testTargets.cmake
├── build-scripts/
│   ├── pkg-support/
│   │   ├── android/
│   │   │   ├── aar/
│   │   │   │   ├── cmake/
│   │   │   │   │   ├── SDL3Config.cmake
│   │   │   │   │   └── SDL3ConfigVersion.cmake.in
│   │   │   │   ├── __main__.py.in
│   │   │   │   └── description.json.in
│   │   │   └── INSTALL.md.in
│   │   ├── mingw/
│   │   │   ├── cmake/
│   │   │   │   ├── SDL3Config.cmake
│   │   │   │   └── SDL3ConfigVersion.cmake
│   │   │   ├── INSTALL.md.in
│   │   │   └── Makefile
│   │   ├── msvc/
│   │   │   ├── arm64/
│   │   │   │   └── INSTALL.md.in
│   │   │   ├── cmake/
│   │   │   │   ├── SDL3Config.cmake.in
│   │   │   │   └── SDL3ConfigVersion.cmake.in
│   │   │   ├── x64/
│   │   │   │   └── INSTALL.md.in
│   │   │   ├── x86/
│   │   │   │   └── INSTALL.md.in
│   │   │   ├── Directory.Build.props
│   │   │   └── INSTALL.md.in
│   │   └── source/
│   │       ├── SDL_revision.h.cmake.in
│   │       └── SDL_revision.h.in
│   ├── add-source-to-projects.pl
│   ├── androidbuildlibs.sh
│   ├── build-release.py
│   ├── build-web-examples.pl
│   ├── casefolding.txt
│   ├── check_android_jni.py
│   ├── check_elf_alignment.sh
│   ├── check_stdlib_usage.py
│   ├── clang-format-src.sh
│   ├── cmake-toolchain-mingw64-i686.cmake
│   ├── cmake-toolchain-mingw64-x86_64.cmake
│   ├── cmake-toolchain-qnx-aarch64le.cmake
│   ├── cmake-toolchain-qnx-x86_64.cmake
│   ├── codechecker-buildbot.sh
│   ├── create_tbds.py
│   ├── create-android-project.py
│   ├── create-release.py
│   ├── djgpp-platform-overrides.cmake
│   ├── fnsince.pl
│   ├── gen_audio_channel_conversion.c
│   ├── git-pre-push-hook.pl
│   ├── harmonyos-tool.sh
│   ├── i586-pc-msdosdjgpp.cmake
│   ├── makecasefoldhashtable.pl
│   ├── release-info.json
│   ├── rename_api.py
│   ├── rename_headers.py
│   ├── rename_macros.py
│   ├── rename_symbols.py
│   ├── rename_types.py
│   ├── SDL_migration.cocci
│   ├── setup-gdk-desktop.py
│   ├── showrev.sh
│   ├── test-versioning.sh
│   ├── update-copyright.sh
│   ├── update-version.sh
│   ├── updaterev.sh
│   └── wikiheaders.pl
├── cmake/
│   ├── android/
│   │   ├── FindSdlAndroid.cmake
│   │   ├── FindSdlAndroidBuildTools.cmake
│   │   ├── FindSdlAndroidPlatform.cmake
│   │   ├── SdlAndroidFunctions.cmake
│   │   └── SdlAndroidScript.cmake
│   ├── test/
│   │   ├── swift/
│   │   │   ├── module.modulemap
│   │   │   └── shim.h
│   │   ├── CMakeLists.txt
│   │   ├── inc_sdl_slash.c
│   │   ├── main_cli.c
│   │   ├── main_gui.c
│   │   ├── main_lib.c
│   │   ├── main.swift
│   │   ├── sdltest.c
│   │   └── test_pkgconfig.sh
│   ├── 3rdparty.cmake
│   ├── cmake_uninstall.cmake.in
│   ├── CPackProjectConfig.cmake.in
│   ├── FindFFmpeg.cmake
│   ├── FindLibUSB.cmake
│   ├── GetGitRevisionDescription.cmake
│   ├── GetGitRevisionDescription.cmake.in
│   ├── macros.cmake
│   ├── PkgConfigHelper.cmake
│   ├── PreseedDOSCache.cmake
│   ├── PreseedEmscriptenCache.cmake
│   ├── PreseedHaikuCache.cmake
│   ├── PreseedMSVCCache.cmake
│   ├── PreseedNokiaNGageCache.cmake
│   ├── PreseedOpenHarmonyCache.cmake
│   ├── sdl3.pc.in
│   ├── SDL3Config.cmake.in
│   ├── SDL3jarTargets.cmake.in
│   ├── sdlchecks.cmake
│   ├── sdlcommands.cmake
│   ├── sdlcompilers.cmake
│   ├── sdlcpu.cmake
│   ├── sdlmanpages.cmake
│   ├── sdlplatform.cmake
│   └── xxd.py
├── docs/
│   ├── doxyfile
│   ├── hello.c
│   ├── INTRO-androidstudio.md
│   ├── INTRO-cmake.md
│   ├── INTRO-emscripten.md
│   ├── INTRO-mingw.md
│   ├── INTRO-visualstudio.md
│   ├── INTRO-xcode.md
│   ├── README-android.md
│   ├── README-bsd.md
│   ├── README-cmake.md
│   ├── README-contributing.md
│   ├── README-documentation-rules.md
│   ├── README-dos.md
│   ├── README-dynapi.md
│   ├── README-emscripten.md
│   ├── README-gdk.md
│   ├── README-haiku.md
│   ├── README-harmonyos.md
│   ├── README-highdpi.md
│   ├── README-ios.md
│   ├── README-kmsbsd.md
│   ├── README-linux.md
│   ├── README-macos.md
│   ├── README-main-functions.md
│   ├── README-migration.md
│   ├── README-n3ds.md
│   ├── README-ngage.md
│   ├── README-platforms.md
│   ├── README-porting.md
│   ├── README-ps2.md
│   ├── README-ps4.md
│   ├── README-ps5.md
│   ├── README-psp.md
│   ├── README-qnx.md
│   ├── README-riscos.md
│   ├── README-steamos.md
│   ├── README-strings.md
│   ├── README-switch.md
│   ├── README-touch.md
│   ├── README-ubuntu-touch.md
│   ├── README-versions.md
│   ├── README-vita.md
│   ├── README-wayland.md
│   ├── README-windows.md
│   ├── README-xr.md
│   ├── README.md
│   └── release_checklist.md
├── Dox/
│   └── ProjTree.md
├── examples/
│   ├── asyncio/
│   │   ├── 01-load-bitmaps/
│   │   │   ├── load-bitmaps.c
│   │   │   ├── README.txt
│   │   │   └── thumbnail.png
│   │   └── description.txt
│   ├── audio/
│   │   ├── 01-simple-playback/
│   │   │   ├── README.txt
│   │   │   └── simple-playback.c
│   │   ├── 02-simple-playback-callback/
│   │   │   ├── README.txt
│   │   │   └── simple-playback-callback.c
│   │   ├── 03-load-wav/
│   │   │   ├── load-wav.c
│   │   │   └── README.txt
│   │   ├── 04-multiple-streams/
│   │   │   ├── multiple-streams.c
│   │   │   └── README.txt
│   │   ├── 05-planar-data/
│   │   │   ├── onmouseover.webp
│   │   │   ├── planar-data.c
│   │   │   ├── README.txt
│   │   │   └── thumbnail.png
│   │   ├── onmouseover.webp
│   │   └── thumbnail.png
│   ├── camera/
│   │   └── 01-read-and-draw/
│   │       ├── onmouseover.webp
│   │       ├── read-and-draw.c
│   │       ├── README.txt
│   │       └── thumbnail.png
│   ├── demo/
│   │   ├── 01-snake/
│   │   │   ├── onmouseover.webp
│   │   │   ├── README.txt
│   │   │   ├── snake.c
│   │   │   └── thumbnail.png
│   │   ├── 02-woodeneye-008/
│   │   │   ├── onmouseover.webp
│   │   │   ├── README.txt
│   │   │   ├── thumbnail.png
│   │   │   └── woodeneye-008.c
│   │   ├── 03-infinite-monkeys/
│   │   │   ├── infinite-monkeys.c
│   │   │   ├── onmouseover.webp
│   │   │   ├── README.txt
│   │   │   └── thumbnail.png
│   │   ├── 04-bytepusher/
│   │   │   ├── bytepusher.c
│   │   │   ├── onmouseover.webp
│   │   │   ├── README.txt
│   │   │   └── thumbnail.png
│   │   └── description.txt
│   ├── input/
│   │   ├── 01-joystick-polling/
│   │   │   ├── joystick-polling.c
│   │   │   ├── onmouseover.webp
│   │   │   ├── README.txt
│   │   │   └── thumbnail.png
│   │   ├── 02-joystick-events/
│   │   │   ├── joystick-events.c
│   │   │   ├── onmouseover.webp
│   │   │   ├── README.txt
│   │   │   └── thumbnail.png
│   │   ├── 03-gamepad-polling/
│   │   │   ├── gamepad-polling.c
│   │   │   ├── onmouseover.webp
│   │   │   ├── README.txt
│   │   │   └── thumbnail.png
│   │   ├── 04-gamepad-events/
│   │   │   ├── gamepad-events.c
│   │   │   ├── onmouseover.webp
│   │   │   ├── README.txt
│   │   │   └── thumbnail.png
│   │   └── 05-gamepad-rumble/
│   │       ├── gamepad-rumble.c
│   │       ├── onmouseover.webp
│   │       ├── README.txt
│   │       └── thumbnail.png
│   ├── misc/
│   │   ├── 01-power/
│   │   │   ├── onmouseover.webp
│   │   │   ├── power.c
│   │   │   ├── README.txt
│   │   │   └── thumbnail.png
│   │   ├── 02-clipboard/
│   │   │   ├── clipboard.c
│   │   │   ├── onmouseover.webp
│   │   │   ├── README.txt
│   │   │   └── thumbnail.png
│   │   ├── 03-locale/
│   │   │   ├── locale.c
│   │   │   ├── README.txt
│   │   │   └── thumbnail.png
│   │   └── description.txt
│   ├── pen/
│   │   └── 01-drawing-lines/
│   │       ├── drawing-lines.c
│   │       ├── onmouseover.webp
│   │       ├── README.txt
│   │       └── thumbnail.png
│   ├── renderer/
│   │   ├── 01-clear/
│   │   │   ├── clear.c
│   │   │   ├── onmouseover.webp
│   │   │   ├── README.txt
│   │   │   └── thumbnail.png
│   │   ├── 02-primitives/
│   │   │   ├── primitives.c
│   │   │   ├── README.txt
│   │   │   └── thumbnail.png
│   │   ├── 03-lines/
│   │   │   ├── lines.c
│   │   │   ├── onmouseover.webp
│   │   │   ├── README.txt
│   │   │   └── thumbnail.png
│   │   ├── 04-points/
│   │   │   ├── onmouseover.webp
│   │   │   ├── points.c
│   │   │   ├── README.txt
│   │   │   └── thumbnail.png
│   │   ├── 05-rectangles/
│   │   │   ├── onmouseover.webp
│   │   │   ├── README.txt
│   │   │   ├── rectangles.c
│   │   │   └── thumbnail.png
│   │   ├── 06-textures/
│   │   │   ├── onmouseover.webp
│   │   │   ├── README.txt
│   │   │   ├── textures.c
│   │   │   └── thumbnail.png
│   │   ├── 07-streaming-textures/
│   │   │   ├── onmouseover.webp
│   │   │   ├── README.txt
│   │   │   ├── streaming-textures.c
│   │   │   └── thumbnail.png
│   │   ├── 08-rotating-textures/
│   │   │   ├── onmouseover.webp
│   │   │   ├── README.txt
│   │   │   ├── rotating-textures.c
│   │   │   └── thumbnail.png
│   │   ├── 09-scaling-textures/
│   │   │   ├── onmouseover.webp
│   │   │   ├── README.txt
│   │   │   ├── scaling-textures.c
│   │   │   └── thumbnail.png
│   │   ├── 10-geometry/
│   │   │   ├── geometry.c
│   │   │   ├── onmouseover.webp
│   │   │   ├── README.txt
│   │   │   └── thumbnail.png
│   │   ├── 11-color-mods/
│   │   │   ├── color-mods.c
│   │   │   ├── onmouseover.webp
│   │   │   ├── README.txt
│   │   │   └── thumbnail.png
│   │   ├── 14-viewport/
│   │   │   ├── README.txt
│   │   │   ├── thumbnail.png
│   │   │   └── viewport.c
│   │   ├── 15-cliprect/
│   │   │   ├── cliprect.c
│   │   │   ├── onmouseover.webp
│   │   │   ├── README.txt
│   │   │   └── thumbnail.png
│   │   ├── 17-read-pixels/
│   │   │   ├── onmouseover.webp
│   │   │   ├── read-pixels.c
│   │   │   ├── README.txt
│   │   │   └── thumbnail.png
│   │   ├── 18-debug-text/
│   │   │   ├── debug-text.c
│   │   │   ├── README.txt
│   │   │   └── thumbnail.png
│   │   ├── 19-affine-textures/
│   │   │   ├── affine-textures.c
│   │   │   ├── onmouseover.webp
│   │   │   ├── README.txt
│   │   │   └── thumbnail.png
│   │   └── 20-blending/
│   │       ├── blending.c
│   │       ├── onmouseover.webp
│   │       ├── README.txt
│   │       └── thumbnail.png
│   ├── storage/
│   │   └── 01-user/
│   │       ├── onmouseover.webp
│   │       ├── README.txt
│   │       ├── thumbnail.png
│   │       └── user.c
│   ├── categories.txt
│   ├── CMakeLists.txt
│   ├── highlight-plugin.lua
│   ├── README.md
│   ├── save-rendering-to-bitmaps.h
│   ├── template-category.html
│   ├── template-homepage.html
│   ├── template-placeholder.png
│   ├── template.c
│   ├── template.css
│   └── template.html
├── include/
│   ├── build_config/
│   │   ├── SDL_build_config_android.h
│   │   ├── SDL_build_config_ios.h
│   │   ├── SDL_build_config_macos.h
│   │   ├── SDL_build_config_minimal.h
│   │   ├── SDL_build_config_windows.h
│   │   ├── SDL_build_config_wingdk.h
│   │   ├── SDL_build_config_xbox.h
│   │   ├── SDL_build_config.h
│   │   ├── SDL_build_config.h.cmake
│   │   └── SDL_revision.h.cmake
│   └── SDL3/
│       ├── SDL_assert.h
│       ├── SDL_asyncio.h
│       ├── SDL_atomic.h
│       ├── SDL_audio.h
│       ├── SDL_begin_code.h
│       ├── SDL_bits.h
│       ├── SDL_blendmode.h
│       ├── SDL_camera.h
│       ├── SDL_clipboard.h
│       ├── SDL_close_code.h
│       ├── SDL_copying.h
│       ├── SDL_cpuinfo.h
│       ├── SDL_dialog.h
│       ├── SDL_dlopennote.h
│       ├── SDL_egl.h
│       ├── SDL_endian.h
│       ├── SDL_error.h
│       ├── SDL_events.h
│       ├── SDL_filesystem.h
│       ├── SDL_gamepad.h
│       ├── SDL_gpu.h
│       ├── SDL_guid.h
│       ├── SDL_haptic.h
│       ├── SDL_hidapi.h
│       ├── SDL_hints.h
│       ├── SDL_init.h
│       ├── SDL_intrin.h
│       ├── SDL_iostream.h
│       ├── SDL_joystick.h
│       ├── SDL_keyboard.h
│       ├── SDL_keycode.h
│       ├── SDL_loadso.h
│       ├── SDL_locale.h
│       ├── SDL_log.h
│       ├── SDL_main_impl.h
│       ├── SDL_main.h
│       ├── SDL_messagebox.h
│       ├── SDL_metal.h
│       ├── SDL_misc.h
│       ├── SDL_mouse.h
│       ├── SDL_mutex.h
│       ├── SDL_notification.h
│       ├── SDL_oldnames.h
│       ├── SDL_opengl_glext.h
│       ├── SDL_opengl.h
│       ├── SDL_opengles.h
│       ├── SDL_opengles2_gl2.h
│       ├── SDL_opengles2_gl2ext.h
│       ├── SDL_opengles2_gl2platform.h
│       ├── SDL_opengles2_khrplatform.h
│       ├── SDL_opengles2.h
│       ├── SDL_openxr.h
│       ├── SDL_pen.h
│       ├── SDL_pixels.h
│       ├── SDL_platform_defines.h
│       ├── SDL_platform.h
│       ├── SDL_power.h
│       ├── SDL_process.h
│       ├── SDL_properties.h
│       ├── SDL_rect.h
│       ├── SDL_render.h
│       ├── SDL_revision.h
│       ├── SDL_scancode.h
│       ├── SDL_sensor.h
│       ├── SDL_stdinc.h
│       ├── SDL_storage.h
│       ├── SDL_surface.h
│       ├── SDL_system.h
│       ├── SDL_test_assert.h
│       ├── SDL_test_common.h
│       ├── SDL_test_compare.h
│       ├── SDL_test_crc32.h
│       ├── SDL_test_font.h
│       ├── SDL_test_fuzzer.h
│       ├── SDL_test_harness.h
│       ├── SDL_test_log.h
│       ├── SDL_test_md5.h
│       ├── SDL_test_memory.h
│       ├── SDL_test.h
│       ├── SDL_thread.h
│       ├── SDL_time.h
│       ├── SDL_timer.h
│       ├── SDL_touch.h
│       ├── SDL_tray.h
│       ├── SDL_version.h
│       ├── SDL_video.h
│       ├── SDL_vulkan.h
│       └── SDL.h
├── openharmony-project/
│   ├── AppScope/
│   │   ├── resources/
│   │   │   └── base/
│   │   │       ├── element/
│   │   │       │   └── string.json
│   │   │       └── media/
│   │   │           ├── background.png
│   │   │           ├── foreground.png
│   │   │           └── layered_image.json
│   │   └── app.json5
│   ├── entry/
│   │   ├── src/
│   │   │   └── main/
│   │   │       ├── cpp/
│   │   │       │   ├── types/
│   │   │       │   │   └── libSDL3/
│   │   │       │   │       ├── Index.d.ts
│   │   │       │   │       └── oh-package.json5
│   │   │       │   └── CMakeLists.txt
│   │   │       ├── ets/
│   │   │       │   ├── entryability/
│   │   │       │   │   └── EntryAbility.ets
│   │   │       │   ├── entrybackupability/
│   │   │       │   │   └── EntryBackupAbility.ets
│   │   │       │   └── pages/
│   │   │       │       └── Index.ets
│   │   │       ├── resources/
│   │   │       │   ├── base/
│   │   │       │   │   ├── element/
│   │   │       │   │   │   ├── color.json
│   │   │       │   │   │   ├── float.json
│   │   │       │   │   │   └── string.json
│   │   │       │   │   ├── media/
│   │   │       │   │   │   ├── background.png
│   │   │       │   │   │   ├── foreground.png
│   │   │       │   │   │   ├── layered_image.json
│   │   │       │   │   │   └── startIcon.png
│   │   │       │   │   └── profile/
│   │   │       │   │       ├── backup_config.json
│   │   │       │   │       └── main_pages.json
│   │   │       │   └── dark/
│   │   │       │       └── element/
│   │   │       │           └── color.json
│   │   │       └── module.json5
│   │   ├── build-profile.json5
│   │   ├── hvigorfile.ts
│   │   ├── obfuscation-rules.txt
│   │   ├── oh-package-lock.json5
│   │   └── oh-package.json5
│   ├── hvigor/
│   │   └── hvigor-config.json5
│   ├── build-profile.json5
│   ├── code-linter.json5
│   ├── hvigorfile.ts
│   ├── oh-package-lock.json5
│   └── oh-package.json5
├── src/
│   ├── atomic/
│   │   ├── SDL_atomic.c
│   │   └── SDL_spinlock.c
│   ├── audio/
│   │   ├── aaudio/
│   │   │   ├── SDL_aaudio.c
│   │   │   ├── SDL_aaudio.h
│   │   │   └── SDL_aaudiofuncs.h
│   │   ├── alsa/
│   │   │   ├── SDL_alsa_audio.c
│   │   │   └── SDL_alsa_audio.h
│   │   ├── coreaudio/
│   │   │   ├── SDL_coreaudio.h
│   │   │   └── SDL_coreaudio.m
│   │   ├── directsound/
│   │   │   ├── SDL_directsound.c
│   │   │   └── SDL_directsound.h
│   │   ├── disk/
│   │   │   ├── SDL_diskaudio.c
│   │   │   └── SDL_diskaudio.h
│   │   ├── dos/
│   │   │   ├── SDL_dosaudio_sb.c
│   │   │   └── SDL_dosaudio_sb.h
│   │   ├── dsp/
│   │   │   ├── SDL_dspaudio.c
│   │   │   └── SDL_dspaudio.h
│   │   ├── dummy/
│   │   │   ├── SDL_dummyaudio.c
│   │   │   └── SDL_dummyaudio.h
│   │   ├── emscripten/
│   │   │   ├── SDL_emscriptenaudio.c
│   │   │   └── SDL_emscriptenaudio.h
│   │   ├── haiku/
│   │   │   ├── SDL_haikuaudio.cc
│   │   │   └── SDL_haikuaudio.h
│   │   ├── jack/
│   │   │   ├── SDL_jackaudio.c
│   │   │   └── SDL_jackaudio.h
│   │   ├── n3ds/
│   │   │   ├── SDL_n3dsaudio.c
│   │   │   └── SDL_n3dsaudio.h
│   │   ├── netbsd/
│   │   │   ├── SDL_netbsdaudio.c
│   │   │   └── SDL_netbsdaudio.h
│   │   ├── ngage/
│   │   │   ├── SDL_ngageaudio.c
│   │   │   ├── SDL_ngageaudio.cpp
│   │   │   ├── SDL_ngageaudio.h
│   │   │   └── SDL_ngageaudio.hpp
│   │   ├── ohaudio/
│   │   │   ├── SDL_ohaudio.c
│   │   │   ├── SDL_ohaudio.h
│   │   │   └── SDL_ohaudiofuncs.h
│   │   ├── openslES/
│   │   │   ├── SDL_openslES.c
│   │   │   └── SDL_openslES.h
│   │   ├── pipewire/
│   │   │   ├── SDL_pipewire.c
│   │   │   └── SDL_pipewire.h
│   │   ├── ps2/
│   │   │   ├── SDL_ps2audio.c
│   │   │   └── SDL_ps2audio.h
│   │   ├── psp/
│   │   │   ├── SDL_pspaudio.c
│   │   │   └── SDL_pspaudio.h
│   │   ├── pulseaudio/
│   │   │   ├── SDL_pulseaudio.c
│   │   │   └── SDL_pulseaudio.h
│   │   ├── qnx/
│   │   │   ├── SDL_qsa_audio.c
│   │   │   └── SDL_qsa_audio.h
│   │   ├── sndio/
│   │   │   ├── SDL_sndioaudio.c
│   │   │   └── SDL_sndioaudio.h
│   │   ├── vita/
│   │   │   ├── SDL_vitaaudio.c
│   │   │   └── SDL_vitaaudio.h
│   │   ├── wasapi/
│   │   │   ├── SDL_wasapi.c
│   │   │   └── SDL_wasapi.h
│   │   ├── SDL_audio_c.h
│   │   ├── SDL_audio_channel_converters.h
│   │   ├── SDL_audio.c
│   │   ├── SDL_audiocvt.c
│   │   ├── SDL_audiodev_c.h
│   │   ├── SDL_audiodev.c
│   │   ├── SDL_audioqueue.c
│   │   ├── SDL_audioqueue.h
│   │   ├── SDL_audioresample.c
│   │   ├── SDL_audioresample.h
│   │   ├── SDL_audiotypecvt.c
│   │   ├── SDL_mixer.c
│   │   ├── SDL_sysaudio.h
│   │   ├── SDL_wave.c
│   │   └── SDL_wave.h
│   ├── camera/
│   │   ├── android/
│   │   │   └── SDL_camera_android.c
│   │   ├── coremedia/
│   │   │   └── SDL_camera_coremedia.m
│   │   ├── dummy/
│   │   │   └── SDL_camera_dummy.c
│   │   ├── emscripten/
│   │   │   └── SDL_camera_emscripten.c
│   │   ├── mediafoundation/
│   │   │   └── SDL_camera_mediafoundation.c
│   │   ├── openharmony/
│   │   │   └── SDL_camera_openharmony.c
│   │   ├── pipewire/
│   │   │   └── SDL_camera_pipewire.c
│   │   ├── v4l2/
│   │   │   └── SDL_camera_v4l2.c
│   │   ├── vita/
│   │   │   └── SDL_camera_vita.c
│   │   ├── SDL_camera_c.h
│   │   ├── SDL_camera.c
│   │   └── SDL_syscamera.h
│   ├── core/
│   │   ├── android/
│   │   │   ├── SDL_android.c
│   │   │   └── SDL_android.h
│   │   ├── dos/
│   │   │   ├── SDL_dos_scheduler.c
│   │   │   ├── SDL_dos_scheduler.h
│   │   │   ├── SDL_dos.c
│   │   │   └── SDL_dos.h
│   │   ├── freebsd/
│   │   │   ├── SDL_evdev_kbd_default_keyaccmap.h
│   │   │   └── SDL_evdev_kbd_freebsd.c
│   │   ├── gdk/
│   │   │   ├── SDL_gdk.cpp
│   │   │   └── SDL_gdk.h
│   │   ├── haiku/
│   │   │   ├── SDL_BApp.h
│   │   │   ├── SDL_BeApp.cc
│   │   │   └── SDL_BeApp.h
│   │   ├── linux/
│   │   │   ├── SDL_dbus.c
│   │   │   ├── SDL_dbus.h
│   │   │   ├── SDL_evdev_capabilities.c
│   │   │   ├── SDL_evdev_capabilities.h
│   │   │   ├── SDL_evdev_kbd_default_accents.h
│   │   │   ├── SDL_evdev_kbd_default_keymap.h
│   │   │   ├── SDL_evdev_kbd.c
│   │   │   ├── SDL_evdev_kbd.h
│   │   │   ├── SDL_evdev.c
│   │   │   ├── SDL_evdev.h
│   │   │   ├── SDL_fcitx.c
│   │   │   ├── SDL_fcitx.h
│   │   │   ├── SDL_ibus.c
│   │   │   ├── SDL_ibus.h
│   │   │   ├── SDL_ime.c
│   │   │   ├── SDL_ime.h
│   │   │   ├── SDL_progressbar.c
│   │   │   ├── SDL_progressbar.h
│   │   │   ├── SDL_system_theme.c
│   │   │   ├── SDL_system_theme.h
│   │   │   ├── SDL_threadprio.c
│   │   │   ├── SDL_ubuntu_touch.c
│   │   │   ├── SDL_ubuntu_touch.h
│   │   │   ├── SDL_udev.c
│   │   │   └── SDL_udev.h
│   │   ├── ngage/
│   │   │   ├── SDL_ngage.cpp
│   │   │   └── SDL_ngage.h
│   │   ├── openbsd/
│   │   │   ├── SDL_wscons_kbd.c
│   │   │   ├── SDL_wscons_mouse.c
│   │   │   └── SDL_wscons.h
│   │   ├── openharmony/
│   │   │   ├── SDL_openharmony.c
│   │   │   └── SDL_openharmony.h
│   │   ├── unix/
│   │   │   ├── SDL_appid.c
│   │   │   ├── SDL_appid.h
│   │   │   ├── SDL_fribidi.c
│   │   │   ├── SDL_fribidi.h
│   │   │   ├── SDL_libthai.c
│   │   │   ├── SDL_libthai.h
│   │   │   ├── SDL_poll.c
│   │   │   └── SDL_poll.h
│   │   ├── windows/
│   │   │   ├── gameinput/
│   │   │   │   ├── gameinput.cpp
│   │   │   │   └── gameinput.h
│   │   │   ├── pch_cpp.cpp
│   │   │   ├── pch.c
│   │   │   ├── SDL_directx.h
│   │   │   ├── SDL_gameinput.cpp
│   │   │   ├── SDL_gameinput.h
│   │   │   ├── SDL_hid.c
│   │   │   ├── SDL_hid.h
│   │   │   ├── SDL_immdevice.c
│   │   │   ├── SDL_immdevice.h
│   │   │   ├── SDL_windows.c
│   │   │   ├── SDL_windows.h
│   │   │   ├── SDL_xinput.c
│   │   │   ├── SDL_xinput.h
│   │   │   └── version.rc
│   │   ├── SDL_core_unsupported.c
│   │   └── SDL_core_unsupported.h
│   ├── cpuinfo/
│   │   ├── SDL_cpuinfo_c.h
│   │   └── SDL_cpuinfo.c
│   ├── dialog/
│   │   ├── android/
│   │   │   └── SDL_androiddialog.c
│   │   ├── cocoa/
│   │   │   └── SDL_cocoadialog.m
│   │   ├── dummy/
│   │   │   └── SDL_dummydialog.c
│   │   ├── haiku/
│   │   │   └── SDL_haikudialog.cc
│   │   ├── unix/
│   │   │   ├── SDL_portaldialog.c
│   │   │   ├── SDL_portaldialog.h
│   │   │   ├── SDL_unixdialog.c
│   │   │   ├── SDL_zenitydialog.c
│   │   │   ├── SDL_zenitydialog.h
│   │   │   ├── SDL_zenitymessagebox.c
│   │   │   └── SDL_zenitymessagebox.h
│   │   ├── windows/
│   │   │   └── SDL_windowsdialog.c
│   │   ├── SDL_dialog_utils.c
│   │   ├── SDL_dialog_utils.h
│   │   ├── SDL_dialog.c
│   │   └── SDL_dialog.h
│   ├── dynapi/
│   │   ├── gendynapi.py
│   │   ├── SDL_dynapi_overrides.h
│   │   ├── SDL_dynapi_procs.h
│   │   ├── SDL_dynapi_unsupported.h
│   │   ├── SDL_dynapi.c
│   │   ├── SDL_dynapi.exports
│   │   ├── SDL_dynapi.h
│   │   └── SDL_dynapi.sym
│   ├── events/
│   │   ├── blank_cursor.h
│   │   ├── default_cursor.h
│   │   ├── imKStoUCS.c
│   │   ├── imKStoUCS.h
│   │   ├── scancodes_darwin.h
│   │   ├── scancodes_linux.h
│   │   ├── scancodes_windows.h
│   │   ├── scancodes_xfree86.h
│   │   ├── SDL_categories_c.h
│   │   ├── SDL_categories.c
│   │   ├── SDL_clipboardevents_c.h
│   │   ├── SDL_clipboardevents.c
│   │   ├── SDL_displayevents_c.h
│   │   ├── SDL_displayevents.c
│   │   ├── SDL_dropevents_c.h
│   │   ├── SDL_dropevents.c
│   │   ├── SDL_eventdescription.c
│   │   ├── SDL_events_c.h
│   │   ├── SDL_events.c
│   │   ├── SDL_eventwatch_c.h
│   │   ├── SDL_eventwatch.c
│   │   ├── SDL_keyboard_c.h
│   │   ├── SDL_keyboard.c
│   │   ├── SDL_keymap_c.h
│   │   ├── SDL_keymap.c
│   │   ├── SDL_keysym_to_keycode_c.h
│   │   ├── SDL_keysym_to_keycode.c
│   │   ├── SDL_keysym_to_scancode_c.h
│   │   ├── SDL_keysym_to_scancode.c
│   │   ├── SDL_mouse_c.h
│   │   ├── SDL_mouse.c
│   │   ├── SDL_notificationevents_c.h
│   │   ├── SDL_notificationevents.c
│   │   ├── SDL_pen_c.h
│   │   ├── SDL_pen.c
│   │   ├── SDL_quit.c
│   │   ├── SDL_scancode_tables_c.h
│   │   ├── SDL_scancode_tables.c
│   │   ├── SDL_touch_c.h
│   │   ├── SDL_touch.c
│   │   ├── SDL_windowevents_c.h
│   │   └── SDL_windowevents.c
│   ├── filesystem/
│   │   ├── android/
│   │   │   └── SDL_sysfilesystem.c
│   │   ├── cocoa/
│   │   │   └── SDL_sysfilesystem.m
│   │   ├── dos/
│   │   │   └── SDL_sysfilesystem.c
│   │   ├── dummy/
│   │   │   ├── SDL_sysfilesystem.c
│   │   │   └── SDL_sysfsops.c
│   │   ├── emscripten/
│   │   │   └── SDL_sysfilesystem.c
│   │   ├── gdk/
│   │   │   └── SDL_sysfilesystem.cpp
│   │   ├── haiku/
│   │   │   └── SDL_sysfilesystem.cc
│   │   ├── n3ds/
│   │   │   └── SDL_sysfilesystem.c
│   │   ├── ngage/
│   │   │   ├── SDL_sysfilesystem.c
│   │   │   └── SDL_sysfilesystem.cpp
│   │   ├── openharmony/
│   │   │   └── SDL_sysfilesystem.c
│   │   ├── posix/
│   │   │   └── SDL_sysfsops.c
│   │   ├── ps2/
│   │   │   └── SDL_sysfilesystem.c
│   │   ├── psp/
│   │   │   └── SDL_sysfilesystem.c
│   │   ├── riscos/
│   │   │   └── SDL_sysfilesystem.c
│   │   ├── unix/
│   │   │   └── SDL_sysfilesystem.c
│   │   ├── vita/
│   │   │   └── SDL_sysfilesystem.c
│   │   ├── windows/
│   │   │   ├── SDL_sysfilesystem.c
│   │   │   └── SDL_sysfsops.c
│   │   ├── SDL_filesystem_c.h
│   │   ├── SDL_filesystem.c
│   │   └── SDL_sysfilesystem.h
│   ├── gpu/
│   │   ├── d3d12/
│   │   │   ├── compile_shaders_xbox.bat
│   │   │   ├── compile_shaders.bat
│   │   │   ├── D3D_Blit.hlsl
│   │   │   ├── D3D12_Blit.h
│   │   │   └── SDL_gpu_d3d12.c
│   │   ├── metal/
│   │   │   ├── compile_shaders.sh
│   │   │   ├── Metal_Blit.h
│   │   │   ├── Metal_Blit.metal
│   │   │   └── SDL_gpu_metal.m
│   │   ├── vulkan/
│   │   │   ├── SDL_gpu_vulkan_vkfuncs.h
│   │   │   └── SDL_gpu_vulkan.c
│   │   ├── xr/
│   │   │   ├── SDL_gpu_openxr.c
│   │   │   ├── SDL_gpu_openxr.h
│   │   │   ├── SDL_openxr_internal.h
│   │   │   ├── SDL_openxrdyn.c
│   │   │   ├── SDL_openxrdyn.h
│   │   │   └── SDL_openxrsym.h
│   │   ├── SDL_gpu_blit.c
│   │   ├── SDL_gpu_copy.c
│   │   ├── SDL_gpu_device.c
│   │   ├── SDL_gpu_format.c
│   │   ├── SDL_gpu.c
│   │   └── SDL_sysgpu.h
│   ├── haptic/
│   │   ├── android/
│   │   │   ├── SDL_syshaptic_c.h
│   │   │   └── SDL_syshaptic.c
│   │   ├── darwin/
│   │   │   ├── SDL_syshaptic_c.h
│   │   │   └── SDL_syshaptic.c
│   │   ├── dummy/
│   │   │   └── SDL_syshaptic.c
│   │   ├── hidapi/
│   │   │   ├── SDL_hidapihaptic_c.h
│   │   │   ├── SDL_hidapihaptic_lg4ff.c
│   │   │   ├── SDL_hidapihaptic.c
│   │   │   └── SDL_hidapihaptic.h
│   │   ├── linux/
│   │   │   └── SDL_syshaptic.c
│   │   ├── windows/
│   │   │   ├── SDL_dinputhaptic_c.h
│   │   │   ├── SDL_dinputhaptic.c
│   │   │   ├── SDL_windowshaptic_c.h
│   │   │   └── SDL_windowshaptic.c
│   │   ├── SDL_haptic_c.h
│   │   ├── SDL_haptic.c
│   │   └── SDL_syshaptic.h
│   ├── hidapi/
│   │   ├── android/
│   │   │   ├── hid.cpp
│   │   │   └── hid.h
│   │   ├── dist/
│   │   │   └── hidapi.podspec
│   │   ├── documentation/
│   │   │   ├── cmake-gui-drop-down.png
│   │   │   └── cmake-gui-highlights.png
│   │   ├── doxygen/
│   │   │   ├── Doxyfile
│   │   │   └── main_page.md
│   │   ├── hidapi/
│   │   │   └── hidapi.h
│   │   ├── hidtest/
│   │   │   ├── CMakeLists.txt
│   │   │   ├── Makefile.am
│   │   │   └── test.c
│   │   ├── ios/
│   │   │   └── hid.m
│   │   ├── libusb/
│   │   │   ├── CMakeLists.txt
│   │   │   ├── hid.c
│   │   │   ├── hidapi_libusb.h
│   │   │   ├── hidapi_thread_pthread.h
│   │   │   ├── hidapi_thread_sdl.h
│   │   │   ├── Makefile-manual
│   │   │   ├── Makefile.am
│   │   │   ├── Makefile.freebsd
│   │   │   ├── Makefile.haiku
│   │   │   └── Makefile.linux
│   │   ├── linux/
│   │   │   ├── CMakeLists.txt
│   │   │   ├── hid.c
│   │   │   ├── Makefile-manual
│   │   │   └── Makefile.am
│   │   ├── m4/
│   │   │   ├── ax_pthread.m4
│   │   │   └── pkg.m4
│   │   ├── mac/
│   │   │   ├── CMakeLists.txt
│   │   │   ├── hid.c
│   │   │   ├── hidapi_darwin.h
│   │   │   ├── Makefile-manual
│   │   │   └── Makefile.am
│   │   ├── netbsd/
│   │   │   ├── CMakeLists.txt
│   │   │   ├── hid.c
│   │   │   └── README.md
│   │   ├── pc/
│   │   │   ├── hidapi-hidraw.pc.in
│   │   │   ├── hidapi-libusb.pc.in
│   │   │   ├── hidapi-netbsd.pc.in
│   │   │   └── hidapi.pc.in
│   │   ├── src/
│   │   │   ├── cmake/
│   │   │   │   └── hidapi-config.cmake.in
│   │   │   └── CMakeLists.txt
│   │   ├── subprojects/
│   │   │   ├── hidapi_build_cmake/
│   │   │   │   └── CMakeLists.txt
│   │   │   └── README.md
│   │   ├── testgui/
│   │   │   ├── TestGUI.app.in/
│   │   │   │   └── Contents/
│   │   │   │       ├── Resources/
│   │   │   │       │   ├── English.lproj/
│   │   │   │       │   │   └── InfoPlist.strings
│   │   │   │       │   └── Signal11.icns
│   │   │   │       ├── Info.plist
│   │   │   │       └── PkgInfo
│   │   │   ├── copy_to_bundle.sh
│   │   │   ├── mac_support_cocoa.m
│   │   │   ├── mac_support.h
│   │   │   ├── Makefile-manual
│   │   │   ├── Makefile.am
│   │   │   ├── Makefile.freebsd
│   │   │   ├── Makefile.linux
│   │   │   ├── Makefile.mac
│   │   │   ├── Makefile.mingw
│   │   │   ├── test.cpp
│   │   │   ├── testgui.sln
│   │   │   └── testgui.vcproj
│   │   ├── udev/
│   │   │   └── 69-hid.rules
│   │   ├── windows/
│   │   │   ├── pp_data_dump/
│   │   │   │   ├── CMakeLists.txt
│   │   │   │   ├── pp_data_dump.c
│   │   │   │   └── README.md
│   │   │   ├── test/
│   │   │   │   ├── data/
│   │   │   │   │   ├── 045E_02FF_0005_0001_expected.rpt_desc
│   │   │   │   │   ├── 045E_02FF_0005_0001_real.rpt_desc
│   │   │   │   │   ├── 045E_02FF_0005_0001.pp_data
│   │   │   │   │   ├── 046A_0011_0006_0001_expected.rpt_desc
│   │   │   │   │   ├── 046A_0011_0006_0001_real.rpt_desc
│   │   │   │   │   ├── 046A_0011_0006_0001.pp_data
│   │   │   │   │   ├── 046D_0A37_0001_000C_expected.rpt_desc
│   │   │   │   │   ├── 046D_0A37_0001_000C_real.rpt_desc
│   │   │   │   │   ├── 046D_0A37_0001_000C.pp_data
│   │   │   │   │   ├── 046D_B010_0001_000C_expected.rpt_desc
│   │   │   │   │   ├── 046D_B010_0001_000C_real.rpt_desc
│   │   │   │   │   ├── 046D_B010_0001_000C.pp_data
│   │   │   │   │   ├── 046D_B010_0001_FF00_expected.rpt_desc
│   │   │   │   │   ├── 046D_B010_0001_FF00_real.rpt_desc
│   │   │   │   │   ├── 046D_B010_0001_FF00.pp_data
│   │   │   │   │   ├── 046D_B010_0002_0001_expected.rpt_desc
│   │   │   │   │   ├── 046D_B010_0002_0001_real.rpt_desc
│   │   │   │   │   ├── 046D_B010_0002_0001.pp_data
│   │   │   │   │   ├── 046D_B010_0002_FF00_expected.rpt_desc
│   │   │   │   │   ├── 046D_B010_0002_FF00_real.rpt_desc
│   │   │   │   │   ├── 046D_B010_0002_FF00.pp_data
│   │   │   │   │   ├── 046D_B010_0006_0001_expected.rpt_desc
│   │   │   │   │   ├── 046D_B010_0006_0001_real.rpt_desc
│   │   │   │   │   ├── 046D_B010_0006_0001.pp_data
│   │   │   │   │   ├── 046D_C077_0002_0001_expected.rpt_desc
│   │   │   │   │   ├── 046D_C077_0002_0001_real.rpt_desc
│   │   │   │   │   ├── 046D_C077_0002_0001.pp_data
│   │   │   │   │   ├── 046D_C283_0004_0001_expected.rpt_desc
│   │   │   │   │   ├── 046D_C283_0004_0001_real.rpt_desc
│   │   │   │   │   ├── 046D_C283_0004_0001.pp_data
│   │   │   │   │   ├── 046D_C52F_0001_000C_expected.rpt_desc
│   │   │   │   │   ├── 046D_C52F_0001_000C_real.rpt_desc
│   │   │   │   │   ├── 046D_C52F_0001_000C.pp_data
│   │   │   │   │   ├── 046D_C52F_0001_FF00_expected.rpt_desc
│   │   │   │   │   ├── 046D_C52F_0001_FF00_real.rpt_desc
│   │   │   │   │   ├── 046D_C52F_0001_FF00.pp_data
│   │   │   │   │   ├── 046D_C52F_0002_0001_expected.rpt_desc
│   │   │   │   │   ├── 046D_C52F_0002_0001_real.rpt_desc
│   │   │   │   │   ├── 046D_C52F_0002_0001.pp_data
│   │   │   │   │   ├── 046D_C52F_0002_FF00_expected.rpt_desc
│   │   │   │   │   ├── 046D_C52F_0002_FF00_real.rpt_desc
│   │   │   │   │   ├── 046D_C52F_0002_FF00.pp_data
│   │   │   │   │   ├── 046D_C534_0001_000C_expected.rpt_desc
│   │   │   │   │   ├── 046D_C534_0001_000C_real.rpt_desc
│   │   │   │   │   ├── 046D_C534_0001_000C.pp_data
│   │   │   │   │   ├── 046D_C534_0001_FF00_expected.rpt_desc
│   │   │   │   │   ├── 046D_C534_0001_FF00_real.rpt_desc
│   │   │   │   │   ├── 046D_C534_0001_FF00.pp_data
│   │   │   │   │   ├── 046D_C534_0002_0001_expected.rpt_desc
│   │   │   │   │   ├── 046D_C534_0002_0001_real.rpt_desc
│   │   │   │   │   ├── 046D_C534_0002_0001.pp_data
│   │   │   │   │   ├── 046D_C534_0002_FF00_expected.rpt_desc
│   │   │   │   │   ├── 046D_C534_0002_FF00_real.rpt_desc
│   │   │   │   │   ├── 046D_C534_0002_FF00.pp_data
│   │   │   │   │   ├── 046D_C534_0006_0001_expected.rpt_desc
│   │   │   │   │   ├── 046D_C534_0006_0001_real.rpt_desc
│   │   │   │   │   ├── 046D_C534_0006_0001.pp_data
│   │   │   │   │   ├── 046D_C534_0080_0001_expected.rpt_desc
│   │   │   │   │   ├── 046D_C534_0080_0001_real.rpt_desc
│   │   │   │   │   ├── 046D_C534_0080_0001.pp_data
│   │   │   │   │   ├── 047F_C056_0001_000C_expected.rpt_desc
│   │   │   │   │   ├── 047F_C056_0001_000C_real.rpt_desc
│   │   │   │   │   ├── 047F_C056_0001_000C.pp_data
│   │   │   │   │   ├── 047F_C056_0003_FFA0_expected.rpt_desc
│   │   │   │   │   ├── 047F_C056_0003_FFA0_real.rpt_desc
│   │   │   │   │   ├── 047F_C056_0003_FFA0.pp_data
│   │   │   │   │   ├── 047F_C056_0005_000B_expected.rpt_desc
│   │   │   │   │   ├── 047F_C056_0005_000B_real.rpt_desc
│   │   │   │   │   ├── 047F_C056_0005_000B.pp_data
│   │   │   │   │   ├── 17CC_1130_0000_FF01_expected.rpt_desc
│   │   │   │   │   ├── 17CC_1130_0000_FF01_real.rpt_desc
│   │   │   │   │   └── 17CC_1130_0000_FF01.pp_data
│   │   │   │   ├── CMakeLists.txt
│   │   │   │   └── hid_report_reconstructor_test.c
│   │   │   ├── CMakeLists.txt
│   │   │   ├── hid.c
│   │   │   ├── hidapi_cfgmgr32.h
│   │   │   ├── hidapi_descriptor_reconstruct.c
│   │   │   ├── hidapi_descriptor_reconstruct.h
│   │   │   ├── hidapi_hidclass.h
│   │   │   ├── hidapi_hidpi.h
│   │   │   ├── hidapi_hidsdi.h
│   │   │   ├── hidapi_winapi.h
│   │   │   ├── hidapi.rc
│   │   │   ├── hidapi.sln
│   │   │   ├── hidapi.vcproj
│   │   │   ├── hidapi.vcxproj
│   │   │   ├── hidtest.vcproj
│   │   │   ├── hidtest.vcxproj
│   │   │   ├── Makefile-manual
│   │   │   ├── Makefile.am
│   │   │   └── Makefile.mingw
│   │   ├── AUTHORS.txt
│   │   ├── bootstrap
│   │   ├── BUILD.autotools.md
│   │   ├── BUILD.cmake.md
│   │   ├── BUILD.md
│   │   ├── CMakeLists.txt
│   │   ├── configure.ac
│   │   ├── HACKING.txt
│   │   ├── LICENSE-bsd.txt
│   │   ├── LICENSE-gpl3.txt
│   │   ├── LICENSE-orig.txt
│   │   ├── LICENSE.txt
│   │   ├── Makefile.am
│   │   ├── meson.build
│   │   ├── README.md
│   │   ├── SDL_hidapi_android.h
│   │   ├── SDL_hidapi_c.h
│   │   ├── SDL_hidapi_ios.h
│   │   ├── SDL_hidapi_libusb.h
│   │   ├── SDL_hidapi_linux.h
│   │   ├── SDL_hidapi_mac.h
│   │   ├── SDL_hidapi_netbsd.h
│   │   ├── SDL_hidapi_steamxbox.h
│   │   ├── SDL_hidapi_windows.h
│   │   ├── SDL_hidapi.c
│   │   └── VERSION
│   ├── io/
│   │   ├── generic/
│   │   │   └── SDL_asyncio_generic.c
│   │   ├── io_uring/
│   │   │   └── SDL_asyncio_liburing.c
│   │   ├── n3ds/
│   │   │   ├── SDL_iostreamromfs.c
│   │   │   └── SDL_iostreamromfs.h
│   │   ├── windows/
│   │   │   └── SDL_asyncio_windows_ioring.c
│   │   ├── SDL_asyncio_c.h
│   │   ├── SDL_asyncio.c
│   │   ├── SDL_iostream_c.h
│   │   ├── SDL_iostream.c
│   │   └── SDL_sysasyncio.h
│   ├── joystick/
│   │   ├── android/
│   │   │   ├── SDL_sysjoystick_c.h
│   │   │   └── SDL_sysjoystick.c
│   │   ├── apple/
│   │   │   ├── SDL_mfijoystick_c.h
│   │   │   └── SDL_mfijoystick.m
│   │   ├── bsd/
│   │   │   └── SDL_bsdjoystick.c
│   │   ├── darwin/
│   │   │   ├── SDL_iokitjoystick_c.h
│   │   │   └── SDL_iokitjoystick.c
│   │   ├── dos/
│   │   │   └── SDL_sysjoystick.c
│   │   ├── dummy/
│   │   │   └── SDL_sysjoystick.c
│   │   ├── emscripten/
│   │   │   ├── SDL_sysjoystick_c.h
│   │   │   └── SDL_sysjoystick.c
│   │   ├── gdk/
│   │   │   └── SDL_gameinputjoystick.cpp
│   │   ├── haiku/
│   │   │   └── SDL_haikujoystick.cc
│   │   ├── hidapi/
│   │   │   ├── steam/
│   │   │   │   ├── controller_constants.h
│   │   │   │   └── controller_structs.h
│   │   │   ├── SDL_hidapi_8bitdo.c
│   │   │   ├── SDL_hidapi_combined.c
│   │   │   ├── SDL_hidapi_flydigi.c
│   │   │   ├── SDL_hidapi_flydigi.h
│   │   │   ├── SDL_hidapi_gamecube.c
│   │   │   ├── SDL_hidapi_gamesir.c
│   │   │   ├── SDL_hidapi_gip.c
│   │   │   ├── SDL_hidapi_lg4ff.c
│   │   │   ├── SDL_hidapi_luna.c
│   │   │   ├── SDL_hidapi_nintendo.h
│   │   │   ├── SDL_hidapi_ps3.c
│   │   │   ├── SDL_hidapi_ps4.c
│   │   │   ├── SDL_hidapi_ps5.c
│   │   │   ├── SDL_hidapi_rumble.c
│   │   │   ├── SDL_hidapi_rumble.h
│   │   │   ├── SDL_hidapi_shield.c
│   │   │   ├── SDL_hidapi_sinput.c
│   │   │   ├── SDL_hidapi_sinput.h
│   │   │   ├── SDL_hidapi_stadia.c
│   │   │   ├── SDL_hidapi_steam_hori.c
│   │   │   ├── SDL_hidapi_steam_triton.c
│   │   │   ├── SDL_hidapi_steam.c
│   │   │   ├── SDL_hidapi_steamdeck.c
│   │   │   ├── SDL_hidapi_switch.c
│   │   │   ├── SDL_hidapi_switch2.c
│   │   │   ├── SDL_hidapi_wii.c
│   │   │   ├── SDL_hidapi_xbox360.c
│   │   │   ├── SDL_hidapi_xbox360.h
│   │   │   ├── SDL_hidapi_xbox360bb.c
│   │   │   ├── SDL_hidapi_xbox360w.c
│   │   │   ├── SDL_hidapi_xboxone.c
│   │   │   ├── SDL_hidapi_zuiki.c
│   │   │   ├── SDL_hidapijoystick_c.h
│   │   │   ├── SDL_hidapijoystick.c
│   │   │   ├── SDL_report_descriptor.c
│   │   │   └── SDL_report_descriptor.h
│   │   ├── linux/
│   │   │   ├── SDL_sysjoystick_c.h
│   │   │   └── SDL_sysjoystick.c
│   │   ├── n3ds/
│   │   │   └── SDL_sysjoystick.c
│   │   ├── ps2/
│   │   │   └── SDL_sysjoystick.c
│   │   ├── psp/
│   │   │   └── SDL_sysjoystick.c
│   │   ├── virtual/
│   │   │   ├── SDL_virtualjoystick_c.h
│   │   │   └── SDL_virtualjoystick.c
│   │   ├── vita/
│   │   │   └── SDL_sysjoystick.c
│   │   ├── windows/
│   │   │   ├── SDL_dinputjoystick_c.h
│   │   │   ├── SDL_dinputjoystick.c
│   │   │   ├── SDL_rawinputjoystick_c.h
│   │   │   ├── SDL_rawinputjoystick.c
│   │   │   ├── SDL_windows_gaming_input.c
│   │   │   ├── SDL_windowsjoystick_c.h
│   │   │   ├── SDL_windowsjoystick.c
│   │   │   ├── SDL_xinputjoystick_c.h
│   │   │   └── SDL_xinputjoystick.c
│   │   ├── check_8bitdo.sh
│   │   ├── controller_list.h
│   │   ├── controller_type.c
│   │   ├── controller_type.h
│   │   ├── SDL_gamepad_c.h
│   │   ├── SDL_gamepad_db.h
│   │   ├── SDL_gamepad_names.c
│   │   ├── SDL_gamepad_sinput.c
│   │   ├── SDL_gamepad.c
│   │   ├── SDL_joystick_c.h
│   │   ├── SDL_joystick_classify.c
│   │   ├── SDL_joystick_guid.c
│   │   ├── SDL_joystick.c
│   │   ├── SDL_steam_virtual_gamepad.c
│   │   ├── SDL_steam_virtual_gamepad.h
│   │   ├── SDL_sysjoystick.h
│   │   ├── SDL_vidpid.c
│   │   ├── sort_controllers.py
│   │   └── usb_ids.h
│   ├── libm/
│   │   ├── e_atan2.c
│   │   ├── e_exp.c
│   │   ├── e_fmod.c
│   │   ├── e_log.c
│   │   ├── e_log10.c
│   │   ├── e_pow.c
│   │   ├── e_rem_pio2.c
│   │   ├── e_sqrt.c
│   │   ├── k_cos.c
│   │   ├── k_rem_pio2.c
│   │   ├── k_sin.c
│   │   ├── k_tan.c
│   │   ├── math_libm.h
│   │   ├── math_private.h
│   │   ├── s_atan.c
│   │   ├── s_copysign.c
│   │   ├── s_cos.c
│   │   ├── s_fabs.c
│   │   ├── s_floor.c
│   │   ├── s_isinf.c
│   │   ├── s_isinff.c
│   │   ├── s_isnan.c
│   │   ├── s_isnanf.c
│   │   ├── s_modf.c
│   │   ├── s_scalbn.c
│   │   ├── s_sin.c
│   │   └── s_tan.c
│   ├── loadso/
│   │   ├── dlopen/
│   │   │   └── SDL_sysloadso.c
│   │   ├── dummy/
│   │   │   └── SDL_sysloadso.c
│   │   └── windows/
│   │       └── SDL_sysloadso.c
│   ├── locale/
│   │   ├── android/
│   │   │   └── SDL_syslocale.c
│   │   ├── dummy/
│   │   │   └── SDL_syslocale.c
│   │   ├── emscripten/
│   │   │   └── SDL_syslocale.c
│   │   ├── haiku/
│   │   │   └── SDL_syslocale.cc
│   │   ├── macos/
│   │   │   └── SDL_syslocale.m
│   │   ├── n3ds/
│   │   │   └── SDL_syslocale.c
│   │   ├── ngage/
│   │   │   └── SDL_syslocale.cpp
│   │   ├── openharmony/
│   │   │   └── SDL_syslocale.c
│   │   ├── psp/
│   │   │   └── SDL_syslocale.c
│   │   ├── unix/
│   │   │   └── SDL_syslocale.c
│   │   ├── vita/
│   │   │   └── SDL_syslocale.c
│   │   ├── windows/
│   │   │   └── SDL_syslocale.c
│   │   ├── SDL_locale.c
│   │   └── SDL_syslocale.h
│   ├── main/
│   │   ├── dos/
│   │   │   └── SDL_sysmain_runapp.c
│   │   ├── emscripten/
│   │   │   ├── SDL_sysmain_callbacks.c
│   │   │   └── SDL_sysmain_runapp.c
│   │   ├── gdk/
│   │   │   └── SDL_sysmain_runapp.cpp
│   │   ├── generic/
│   │   │   └── SDL_sysmain_callbacks.c
│   │   ├── ios/
│   │   │   └── SDL_sysmain_callbacks.m
│   │   ├── n3ds/
│   │   │   └── SDL_sysmain_runapp.c
│   │   ├── ngage/
│   │   │   ├── SDL_sysmain_callbacks.c
│   │   │   ├── SDL_sysmain_main.cpp
│   │   │   └── SDL_sysmain_main.hpp
│   │   ├── openharmony/
│   │   │   ├── SDL_sysmain_callbacks.c
│   │   │   └── SDL_sysmain_runapp.c
│   │   ├── ps2/
│   │   │   └── SDL_sysmain_runapp.c
│   │   ├── psp/
│   │   │   └── SDL_sysmain_runapp.c
│   │   ├── windows/
│   │   │   └── SDL_sysmain_runapp.c
│   │   ├── SDL_main_callbacks.c
│   │   ├── SDL_main_callbacks.h
│   │   └── SDL_runapp.c
│   ├── misc/
│   │   ├── android/
│   │   │   └── SDL_sysurl.c
│   │   ├── dummy/
│   │   │   └── SDL_sysurl.c
│   │   ├── emscripten/
│   │   │   └── SDL_sysurl.c
│   │   ├── haiku/
│   │   │   └── SDL_sysurl.cc
│   │   ├── ios/
│   │   │   └── SDL_sysurl.m
│   │   ├── macos/
│   │   │   └── SDL_sysurl.m
│   │   ├── openharmony/
│   │   │   └── SDL_sysurl.c
│   │   ├── riscos/
│   │   │   └── SDL_sysurl.c
│   │   ├── unix/
│   │   │   └── SDL_sysurl.c
│   │   ├── vita/
│   │   │   └── SDL_sysurl.c
│   │   ├── windows/
│   │   │   └── SDL_sysurl.c
│   │   ├── SDL_libusb.c
│   │   ├── SDL_libusb.h
│   │   ├── SDL_sysurl.h
│   │   └── SDL_url.c
│   ├── notification/
│   │   ├── cocoa/
│   │   │   └── SDL_cocoanotification.m
│   │   ├── dummy/
│   │   │   └── SDL_dummynotification.c
│   │   ├── unix/
│   │   │   └── SDL_dbusnotification.c
│   │   ├── windows/
│   │   │   └── SDL_windowsnotification.c
│   │   ├── SDL_notification_c.h
│   │   └── SDL_notification.c
│   ├── power/
│   │   ├── android/
│   │   │   └── SDL_syspower.c
│   │   ├── emscripten/
│   │   │   └── SDL_syspower.c
│   │   ├── haiku/
│   │   │   └── SDL_syspower.c
│   │   ├── linux/
│   │   │   └── SDL_syspower.c
│   │   ├── macos/
│   │   │   └── SDL_syspower.c
│   │   ├── n3ds/
│   │   │   └── SDL_syspower.c
│   │   ├── openharmony/
│   │   │   └── SDL_syspower.c
│   │   ├── psp/
│   │   │   └── SDL_syspower.c
│   │   ├── uikit/
│   │   │   ├── SDL_syspower.h
│   │   │   └── SDL_syspower.m
│   │   ├── vita/
│   │   │   └── SDL_syspower.c
│   │   ├── windows/
│   │   │   └── SDL_syspower.c
│   │   ├── SDL_power.c
│   │   └── SDL_syspower.h
│   ├── process/
│   │   ├── dummy/
│   │   │   └── SDL_dummyprocess.c
│   │   ├── posix/
│   │   │   └── SDL_posixprocess.c
│   │   ├── windows/
│   │   │   └── SDL_windowsprocess.c
│   │   ├── SDL_process.c
│   │   └── SDL_sysprocess.h
│   ├── render/
│   │   ├── direct3d/
│   │   │   ├── compile_shaders.bat
│   │   │   ├── D3D9_PixelShader_Palette_Linear.h
│   │   │   ├── D3D9_PixelShader_Palette_Linear.hlsl
│   │   │   ├── D3D9_PixelShader_Palette_Nearest.h
│   │   │   ├── D3D9_PixelShader_Palette_Nearest.hlsl
│   │   │   ├── D3D9_PixelShader_Palette.h
│   │   │   ├── D3D9_PixelShader_Palette.hlsli
│   │   │   ├── D3D9_PixelShader_YUV.h
│   │   │   ├── D3D9_PixelShader_YUV.hlsl
│   │   │   ├── SDL_render_d3d.c
│   │   │   ├── SDL_shaders_d3d.c
│   │   │   └── SDL_shaders_d3d.h
│   │   ├── direct3d11/
│   │   │   ├── compile_shaders.bat
│   │   │   ├── D3D11_PixelShader_Advanced.h
│   │   │   ├── D3D11_PixelShader_Advanced.hlsl
│   │   │   ├── D3D11_PixelShader_Colors_PQ.h
│   │   │   ├── D3D11_PixelShader_Colors_PQ.hlsl
│   │   │   ├── D3D11_PixelShader_Colors.h
│   │   │   ├── D3D11_PixelShader_Colors.hlsl
│   │   │   ├── D3D11_PixelShader_Common.hlsli
│   │   │   ├── D3D11_PixelShader_Textures_PQ.h
│   │   │   ├── D3D11_PixelShader_Textures_PQ.hlsl
│   │   │   ├── D3D11_PixelShader_Textures_Simple.h
│   │   │   ├── D3D11_PixelShader_Textures_Simple.hlsl
│   │   │   ├── D3D11_PixelShader_Textures.h
│   │   │   ├── D3D11_PixelShader_Textures.hlsl
│   │   │   ├── D3D11_VertexShader.h
│   │   │   ├── D3D11_VertexShader.hlsl
│   │   │   ├── SDL_render_d3d11.c
│   │   │   ├── SDL_shaders_d3d11.c
│   │   │   └── SDL_shaders_d3d11.h
│   │   ├── direct3d12/
│   │   │   ├── compile_shaders_xbox.bat
│   │   │   ├── compile_shaders.bat
│   │   │   ├── D3D12_PixelShader_Advanced.h
│   │   │   ├── D3D12_PixelShader_Advanced.hlsl
│   │   │   ├── D3D12_PixelShader_Colors_PQ.h
│   │   │   ├── D3D12_PixelShader_Colors_PQ.hlsl
│   │   │   ├── D3D12_PixelShader_Colors.h
│   │   │   ├── D3D12_PixelShader_Colors.hlsl
│   │   │   ├── D3D12_PixelShader_Common.hlsli
│   │   │   ├── D3D12_PixelShader_Textures_PQ.h
│   │   │   ├── D3D12_PixelShader_Textures_PQ.hlsl
│   │   │   ├── D3D12_PixelShader_Textures_Simple.h
│   │   │   ├── D3D12_PixelShader_Textures_Simple.hlsl
│   │   │   ├── D3D12_PixelShader_Textures.h
│   │   │   ├── D3D12_PixelShader_Textures.hlsl
│   │   │   ├── D3D12_RootSig_Advanced.h
│   │   │   ├── D3D12_RootSig_Color.h
│   │   │   ├── D3D12_RootSig_Texture.h
│   │   │   ├── D3D12_Shader_Common.hlsli
│   │   │   ├── D3D12_VertexShader_Advanced.h
│   │   │   ├── D3D12_VertexShader_Color.h
│   │   │   ├── D3D12_VertexShader_Texture.h
│   │   │   ├── D3D12_VertexShader.hlsl
│   │   │   ├── SDL_render_d3d12_xbox.cpp
│   │   │   ├── SDL_render_d3d12_xbox.h
│   │   │   ├── SDL_render_d3d12.c
│   │   │   ├── SDL_shaders_d3d12_xboxone.cpp
│   │   │   ├── SDL_shaders_d3d12_xboxseries.cpp
│   │   │   ├── SDL_shaders_d3d12.c
│   │   │   └── SDL_shaders_d3d12.h
│   │   ├── gpu/
│   │   │   ├── shaders/
│   │   │   │   ├── build-shaders.sh
│   │   │   │   ├── color.frag.dxil.h
│   │   │   │   ├── color.frag.hlsl
│   │   │   │   ├── color.frag.msl.h
│   │   │   │   ├── color.frag.spv.h
│   │   │   │   ├── common.frag.hlsli
│   │   │   │   ├── dxil.h
│   │   │   │   ├── linepoint.vert.dxil.h
│   │   │   │   ├── linepoint.vert.hlsl
│   │   │   │   ├── linepoint.vert.msl.h
│   │   │   │   ├── linepoint.vert.spv.h
│   │   │   │   ├── msl.h
│   │   │   │   ├── spir-v.h
│   │   │   │   ├── texture_advanced.frag.dxil.h
│   │   │   │   ├── texture_advanced.frag.hlsl
│   │   │   │   ├── texture_advanced.frag.msl.h
│   │   │   │   ├── texture_advanced.frag.spv.h
│   │   │   │   ├── texture_rgb.frag.dxil.h
│   │   │   │   ├── texture_rgb.frag.hlsl
│   │   │   │   ├── texture_rgb.frag.msl.h
│   │   │   │   ├── texture_rgb.frag.spv.h
│   │   │   │   ├── texture_rgba.frag.dxil.h
│   │   │   │   ├── texture_rgba.frag.hlsl
│   │   │   │   ├── texture_rgba.frag.msl.h
│   │   │   │   ├── texture_rgba.frag.spv.h
│   │   │   │   ├── tri_color.vert.dxil.h
│   │   │   │   ├── tri_color.vert.hlsl
│   │   │   │   ├── tri_color.vert.msl.h
│   │   │   │   ├── tri_color.vert.spv.h
│   │   │   │   ├── tri_texture.vert.dxil.h
│   │   │   │   ├── tri_texture.vert.hlsl
│   │   │   │   ├── tri_texture.vert.msl.h
│   │   │   │   └── tri_texture.vert.spv.h
│   │   │   ├── SDL_gpu_util.h
│   │   │   ├── SDL_pipeline_gpu.c
│   │   │   ├── SDL_pipeline_gpu.h
│   │   │   ├── SDL_render_gpu.c
│   │   │   ├── SDL_shaders_gpu.c
│   │   │   └── SDL_shaders_gpu.h
│   │   ├── metal/
│   │   │   ├── build-metal-shaders.sh
│   │   │   ├── SDL_render_metal.m
│   │   │   ├── SDL_shaders_metal_ios.h
│   │   │   ├── SDL_shaders_metal_iphonesimulator.h
│   │   │   ├── SDL_shaders_metal_macos.h
│   │   │   ├── SDL_shaders_metal_tvos.h
│   │   │   ├── SDL_shaders_metal_tvsimulator.h
│   │   │   └── SDL_shaders_metal.metal
│   │   ├── ngage/
│   │   │   ├── SDL_render_ngage_c.h
│   │   │   ├── SDL_render_ngage_c.hpp
│   │   │   ├── SDL_render_ngage.c
│   │   │   ├── SDL_render_ngage.cpp
│   │   │   ├── SDL_render_ops.cpp
│   │   │   └── SDL_render_ops.hpp
│   │   ├── opengl/
│   │   │   ├── SDL_glfuncs.h
│   │   │   ├── SDL_render_gl.c
│   │   │   ├── SDL_shaders_gl.c
│   │   │   └── SDL_shaders_gl.h
│   │   ├── opengles/
│   │   │   ├── SDL_glesfuncs.h
│   │   │   └── SDL_render_gles.c
│   │   ├── opengles2/
│   │   │   ├── SDL_gles2funcs.h
│   │   │   ├── SDL_render_gles2.c
│   │   │   ├── SDL_shaders_gles2.c
│   │   │   └── SDL_shaders_gles2.h
│   │   ├── ps2/
│   │   │   └── SDL_render_ps2.c
│   │   ├── psp/
│   │   │   ├── SDL_render_psp_c.h
│   │   │   └── SDL_render_psp.c
│   │   ├── software/
│   │   │   ├── SDL_blendfillrect.c
│   │   │   ├── SDL_blendfillrect.h
│   │   │   ├── SDL_blendline.c
│   │   │   ├── SDL_blendline.h
│   │   │   ├── SDL_blendpoint.c
│   │   │   ├── SDL_blendpoint.h
│   │   │   ├── SDL_draw.h
│   │   │   ├── SDL_drawline.c
│   │   │   ├── SDL_drawline.h
│   │   │   ├── SDL_drawpoint.c
│   │   │   ├── SDL_drawpoint.h
│   │   │   ├── SDL_render_sw_c.h
│   │   │   ├── SDL_render_sw.c
│   │   │   ├── SDL_triangle.c
│   │   │   └── SDL_triangle.h
│   │   ├── vitagxm/
│   │   │   ├── shader_src/
│   │   │   │   ├── clear_f.cg
│   │   │   │   ├── clear_v.cg
│   │   │   │   ├── color_f.cg
│   │   │   │   ├── color_v.cg
│   │   │   │   ├── texture_f.cg
│   │   │   │   └── texture_v.cg
│   │   │   ├── SDL_render_vita_gxm_memory.c
│   │   │   ├── SDL_render_vita_gxm_memory.h
│   │   │   ├── SDL_render_vita_gxm_shaders.h
│   │   │   ├── SDL_render_vita_gxm_tools.c
│   │   │   ├── SDL_render_vita_gxm_tools.h
│   │   │   ├── SDL_render_vita_gxm_types.h
│   │   │   └── SDL_render_vita_gxm.c
│   │   ├── vulkan/
│   │   │   ├── compile_shaders.bat
│   │   │   ├── SDL_render_vulkan.c
│   │   │   ├── SDL_shaders_vulkan.c
│   │   │   ├── SDL_shaders_vulkan.h
│   │   │   ├── VULKAN_PixelShader_Advanced.h
│   │   │   ├── VULKAN_PixelShader_Advanced.hlsl
│   │   │   ├── VULKAN_PixelShader_Colors_PQ.h
│   │   │   ├── VULKAN_PixelShader_Colors_PQ.hlsl
│   │   │   ├── VULKAN_PixelShader_Colors.h
│   │   │   ├── VULKAN_PixelShader_Colors.hlsl
│   │   │   ├── VULKAN_PixelShader_Common.hlsli
│   │   │   ├── VULKAN_PixelShader_Textures_PQ.h
│   │   │   ├── VULKAN_PixelShader_Textures_PQ.hlsl
│   │   │   ├── VULKAN_PixelShader_Textures_Simple.h
│   │   │   ├── VULKAN_PixelShader_Textures_Simple.hlsl
│   │   │   ├── VULKAN_PixelShader_Textures.h
│   │   │   ├── VULKAN_PixelShader_Textures.hlsl
│   │   │   ├── VULKAN_VertexShader.h
│   │   │   └── VULKAN_VertexShader.hlsl
│   │   ├── SDL_blendmode.c
│   │   ├── SDL_d3dmath.h
│   │   ├── SDL_render_debug_font.h
│   │   ├── SDL_render_debug.c
│   │   ├── SDL_render_texture_state.c
│   │   ├── SDL_render_texture_update.c
│   │   ├── SDL_render_unsupported.c
│   │   ├── SDL_render.c
│   │   ├── SDL_sysrender.h
│   │   ├── SDL_yuv_sw_c.h
│   │   └── SDL_yuv_sw.c
│   ├── sensor/
│   │   ├── android/
│   │   │   ├── SDL_androidsensor.c
│   │   │   └── SDL_androidsensor.h
│   │   ├── coremotion/
│   │   │   ├── SDL_coremotionsensor.h
│   │   │   └── SDL_coremotionsensor.m
│   │   ├── dummy/
│   │   │   ├── SDL_dummysensor.c
│   │   │   └── SDL_dummysensor.h
│   │   ├── emscripten/
│   │   │   ├── SDL_emscriptensensor.c
│   │   │   └── SDL_emscriptensensor.h
│   │   ├── n3ds/
│   │   │   └── SDL_n3dssensor.c
│   │   ├── vita/
│   │   │   ├── SDL_vitasensor.c
│   │   │   └── SDL_vitasensor.h
│   │   ├── windows/
│   │   │   ├── SDL_windowssensor.c
│   │   │   └── SDL_windowssensor.h
│   │   ├── SDL_sensor_c.h
│   │   ├── SDL_sensor.c
│   │   └── SDL_syssensor.h
│   ├── stdlib/
│   │   ├── SDL_casefolding.h
│   │   ├── SDL_crc16.c
│   │   ├── SDL_crc32.c
│   │   ├── SDL_getenv_c.h
│   │   ├── SDL_getenv.c
│   │   ├── SDL_iconv.c
│   │   ├── SDL_malloc.c
│   │   ├── SDL_memcpy.c
│   │   ├── SDL_memmove.c
│   │   ├── SDL_memset.c
│   │   ├── SDL_mslibc_arm64.masm
│   │   ├── SDL_mslibc_x64.masm
│   │   ├── SDL_mslibc.c
│   │   ├── SDL_murmur3.c
│   │   ├── SDL_qsort.c
│   │   ├── SDL_random.c
│   │   ├── SDL_stdlib.c
│   │   ├── SDL_string.c
│   │   ├── SDL_strtokr.c
│   │   ├── SDL_sysstdlib.h
│   │   └── SDL_vacopy.h
│   ├── storage/
│   │   ├── generic/
│   │   │   └── SDL_genericstorage.c
│   │   ├── steam/
│   │   │   ├── SDL_steamstorage_proc.h
│   │   │   └── SDL_steamstorage.c
│   │   ├── SDL_storage.c
│   │   └── SDL_sysstorage.h
│   ├── test/
│   │   ├── SDL_test_assert.c
│   │   ├── SDL_test_common.c
│   │   ├── SDL_test_compare.c
│   │   ├── SDL_test_crc32.c
│   │   ├── SDL_test_font.c
│   │   ├── SDL_test_fuzzer.c
│   │   ├── SDL_test_harness.c
│   │   ├── SDL_test_internal.h
│   │   ├── SDL_test_log.c
│   │   ├── SDL_test_md5.c
│   │   └── SDL_test_memory.c
│   ├── thread/
│   │   ├── dos/
│   │   │   ├── SDL_sysmutex.c
│   │   │   ├── SDL_syssem.c
│   │   │   ├── SDL_systhread_c.h
│   │   │   ├── SDL_systhread.c
│   │   │   └── SDL_systls.c
│   │   ├── generic/
│   │   │   ├── SDL_syscond_c.h
│   │   │   ├── SDL_syscond.c
│   │   │   ├── SDL_sysmutex_c.h
│   │   │   ├── SDL_sysmutex.c
│   │   │   ├── SDL_sysrwlock_c.h
│   │   │   ├── SDL_sysrwlock.c
│   │   │   ├── SDL_syssem.c
│   │   │   ├── SDL_systhread_c.h
│   │   │   ├── SDL_systhread.c
│   │   │   └── SDL_systls.c
│   │   ├── n3ds/
│   │   │   ├── SDL_sysmutex_c.h
│   │   │   ├── SDL_sysmutex.c
│   │   │   ├── SDL_syssem.c
│   │   │   ├── SDL_systhread_c.h
│   │   │   └── SDL_systhread.c
│   │   ├── ps2/
│   │   │   ├── SDL_syssem.c
│   │   │   ├── SDL_systhread_c.h
│   │   │   └── SDL_systhread.c
│   │   ├── psp/
│   │   │   ├── SDL_sysmutex_c.h
│   │   │   ├── SDL_sysmutex.c
│   │   │   ├── SDL_syssem.c
│   │   │   ├── SDL_systhread_c.h
│   │   │   └── SDL_systhread.c
│   │   ├── pthread/
│   │   │   ├── SDL_syscond.c
│   │   │   ├── SDL_sysmutex_c.h
│   │   │   ├── SDL_sysmutex.c
│   │   │   ├── SDL_sysrwlock.c
│   │   │   ├── SDL_syssem.c
│   │   │   ├── SDL_systhread_c.h
│   │   │   ├── SDL_systhread.c
│   │   │   └── SDL_systls.c
│   │   ├── vita/
│   │   │   ├── SDL_sysmutex_c.h
│   │   │   ├── SDL_sysmutex.c
│   │   │   ├── SDL_syssem.c
│   │   │   ├── SDL_systhread_c.h
│   │   │   └── SDL_systhread.c
│   │   ├── windows/
│   │   │   ├── SDL_syscond_cv.c
│   │   │   ├── SDL_sysmutex_c.h
│   │   │   ├── SDL_sysmutex.c
│   │   │   ├── SDL_sysrwlock_srw.c
│   │   │   ├── SDL_syssem.c
│   │   │   ├── SDL_systhread_c.h
│   │   │   ├── SDL_systhread.c
│   │   │   └── SDL_systls.c
│   │   ├── SDL_systhread.h
│   │   ├── SDL_thread_c.h
│   │   └── SDL_thread.c
│   ├── time/
│   │   ├── dummy/
│   │   │   └── SDL_systime.c
│   │   ├── n3ds/
│   │   │   └── SDL_systime.c
│   │   ├── ngage/
│   │   │   └── SDL_systime.cpp
│   │   ├── psp/
│   │   │   └── SDL_systime.c
│   │   ├── unix/
│   │   │   └── SDL_systime.c
│   │   ├── vita/
│   │   │   └── SDL_systime.c
│   │   ├── windows/
│   │   │   └── SDL_systime.c
│   │   ├── SDL_time_c.h
│   │   └── SDL_time.c
│   ├── timer/
│   │   ├── dos/
│   │   │   └── SDL_systimer.c
│   │   ├── haiku/
│   │   │   └── SDL_systimer.c
│   │   ├── n3ds/
│   │   │   └── SDL_systimer.c
│   │   ├── ngage/
│   │   │   └── SDL_systimer.cpp
│   │   ├── ps2/
│   │   │   └── SDL_systimer.c
│   │   ├── psp/
│   │   │   └── SDL_systimer.c
│   │   ├── unix/
│   │   │   └── SDL_systimer.c
│   │   ├── vita/
│   │   │   └── SDL_systimer.c
│   │   ├── windows/
│   │   │   └── SDL_systimer.c
│   │   ├── SDL_timer_c.h
│   │   └── SDL_timer.c
│   ├── tray/
│   │   ├── cocoa/
│   │   │   └── SDL_tray.m
│   │   ├── dummy/
│   │   │   └── SDL_tray.c
│   │   ├── unix/
│   │   │   ├── SDL_dbustray.c
│   │   │   ├── SDL_tray.c
│   │   │   └── SDL_unixtray.h
│   │   ├── windows/
│   │   │   └── SDL_tray.c
│   │   ├── SDL_tray_utils.c
│   │   └── SDL_tray_utils.h
│   ├── video/
│   │   ├── android/
│   │   │   ├── SDL_androidclipboard.c
│   │   │   ├── SDL_androidclipboard.h
│   │   │   ├── SDL_androidevents.c
│   │   │   ├── SDL_androidevents.h
│   │   │   ├── SDL_androidgl.c
│   │   │   ├── SDL_androidgl.h
│   │   │   ├── SDL_androidkeyboard.c
│   │   │   ├── SDL_androidkeyboard.h
│   │   │   ├── SDL_androidmessagebox.c
│   │   │   ├── SDL_androidmessagebox.h
│   │   │   ├── SDL_androidmouse.c
│   │   │   ├── SDL_androidmouse.h
│   │   │   ├── SDL_androidpen.c
│   │   │   ├── SDL_androidpen.h
│   │   │   ├── SDL_androidtouch.c
│   │   │   ├── SDL_androidtouch.h
│   │   │   ├── SDL_androidvideo.c
│   │   │   ├── SDL_androidvideo.h
│   │   │   ├── SDL_androidvulkan.c
│   │   │   ├── SDL_androidvulkan.h
│   │   │   ├── SDL_androidwindow.c
│   │   │   └── SDL_androidwindow.h
│   │   ├── arm/
│   │   │   ├── SDL_sve2_blit_A.c
│   │   │   ├── SDL_sve2_blit_A.h
│   │   │   ├── SDL_sve2_blit_N.c
│   │   │   ├── SDL_sve2_blit_N.h
│   │   │   ├── SDL_sve2_extension.h
│   │   │   ├── SDL_sve2_swizzle.h
│   │   │   └── SDL_sve2_util.h
│   │   ├── cocoa/
│   │   │   ├── SDL_cocoaclipboard.h
│   │   │   ├── SDL_cocoaclipboard.m
│   │   │   ├── SDL_cocoaevents.h
│   │   │   ├── SDL_cocoaevents.m
│   │   │   ├── SDL_cocoakeyboard.h
│   │   │   ├── SDL_cocoakeyboard.m
│   │   │   ├── SDL_cocoamessagebox.h
│   │   │   ├── SDL_cocoamessagebox.m
│   │   │   ├── SDL_cocoametalview.h
│   │   │   ├── SDL_cocoametalview.m
│   │   │   ├── SDL_cocoamodes.h
│   │   │   ├── SDL_cocoamodes.m
│   │   │   ├── SDL_cocoamouse.h
│   │   │   ├── SDL_cocoamouse.m
│   │   │   ├── SDL_cocoaopengl.h
│   │   │   ├── SDL_cocoaopengl.m
│   │   │   ├── SDL_cocoaopengles.h
│   │   │   ├── SDL_cocoaopengles.m
│   │   │   ├── SDL_cocoapen.h
│   │   │   ├── SDL_cocoapen.m
│   │   │   ├── SDL_cocoashape.h
│   │   │   ├── SDL_cocoashape.m
│   │   │   ├── SDL_cocoavideo.h
│   │   │   ├── SDL_cocoavideo.m
│   │   │   ├── SDL_cocoavulkan.h
│   │   │   ├── SDL_cocoavulkan.m
│   │   │   ├── SDL_cocoawindow.h
│   │   │   └── SDL_cocoawindow.m
│   │   ├── directx/
│   │   │   ├── d3d12.h
│   │   │   ├── d3d12sdklayers.h
│   │   │   ├── gen_xbox_cmacros.cs
│   │   │   ├── SDL_d3d12_xbox_cmacros.h
│   │   │   └── SDL_d3d12.h
│   │   ├── dos/
│   │   │   ├── SDL_dosevents_c.h
│   │   │   ├── SDL_dosevents.c
│   │   │   ├── SDL_dosframebuffer_c.h
│   │   │   ├── SDL_dosframebuffer.c
│   │   │   ├── SDL_dosmodes.c
│   │   │   ├── SDL_dosmodes.h
│   │   │   ├── SDL_dosmouse.c
│   │   │   ├── SDL_dosmouse.h
│   │   │   ├── SDL_dosvideo.c
│   │   │   └── SDL_dosvideo.h
│   │   ├── dummy/
│   │   │   ├── SDL_nullevents_c.h
│   │   │   ├── SDL_nullevents.c
│   │   │   ├── SDL_nullframebuffer_c.h
│   │   │   ├── SDL_nullframebuffer.c
│   │   │   ├── SDL_nullvideo.c
│   │   │   └── SDL_nullvideo.h
│   │   ├── emscripten/
│   │   │   ├── SDL_emscriptenevents.c
│   │   │   ├── SDL_emscriptenevents.h
│   │   │   ├── SDL_emscriptenframebuffer.c
│   │   │   ├── SDL_emscriptenframebuffer.h
│   │   │   ├── SDL_emscriptenmouse.c
│   │   │   ├── SDL_emscriptenmouse.h
│   │   │   ├── SDL_emscriptenopengles.c
│   │   │   ├── SDL_emscriptenopengles.h
│   │   │   ├── SDL_emscriptenvideo.c
│   │   │   └── SDL_emscriptenvideo.h
│   │   ├── gdk/
│   │   │   ├── SDL_gdktextinput.cpp
│   │   │   └── SDL_gdktextinput.h
│   │   ├── haiku/
│   │   │   ├── SDL_bclipboard.cc
│   │   │   ├── SDL_bclipboard.h
│   │   │   ├── SDL_bevents.cc
│   │   │   ├── SDL_bevents.h
│   │   │   ├── SDL_bframebuffer.cc
│   │   │   ├── SDL_bframebuffer.h
│   │   │   ├── SDL_bkeyboard.cc
│   │   │   ├── SDL_bkeyboard.h
│   │   │   ├── SDL_bmessagebox.cc
│   │   │   ├── SDL_bmessagebox.h
│   │   │   ├── SDL_bmodes.cc
│   │   │   ├── SDL_bmodes.h
│   │   │   ├── SDL_bopengl.cc
│   │   │   ├── SDL_bopengl.h
│   │   │   ├── SDL_bvideo.cc
│   │   │   ├── SDL_bvideo.h
│   │   │   ├── SDL_BWin.h
│   │   │   ├── SDL_bwindow.cc
│   │   │   └── SDL_bwindow.h
│   │   ├── khronos/
│   │   │   ├── EGL/
│   │   │   │   ├── egl.h
│   │   │   │   ├── eglext.h
│   │   │   │   └── eglplatform.h
│   │   │   ├── GLES/
│   │   │   │   ├── egl.h
│   │   │   │   ├── gl.h
│   │   │   │   ├── glext.h
│   │   │   │   └── glplatform.h
│   │   │   ├── GLES2/
│   │   │   │   ├── gl2.h
│   │   │   │   ├── gl2ext.h
│   │   │   │   └── gl2platform.h
│   │   │   ├── GLES3/
│   │   │   │   ├── gl3.h
│   │   │   │   ├── gl31.h
│   │   │   │   ├── gl32.h
│   │   │   │   └── gl3platform.h
│   │   │   ├── KHR/
│   │   │   │   └── khrplatform.h
│   │   │   ├── openxr/
│   │   │   │   ├── openxr_loader_negotiation.h
│   │   │   │   ├── openxr_platform_defines.h
│   │   │   │   ├── openxr_platform.h
│   │   │   │   ├── openxr_reflection_parent_structs.h
│   │   │   │   ├── openxr_reflection_structs.h
│   │   │   │   ├── openxr_reflection.h
│   │   │   │   └── openxr.h
│   │   │   ├── vk_video/
│   │   │   │   ├── vulkan_video_codec_av1std_decode.h
│   │   │   │   ├── vulkan_video_codec_av1std_encode.h
│   │   │   │   ├── vulkan_video_codec_av1std.h
│   │   │   │   ├── vulkan_video_codec_h264std_decode.h
│   │   │   │   ├── vulkan_video_codec_h264std_encode.h
│   │   │   │   ├── vulkan_video_codec_h264std.h
│   │   │   │   ├── vulkan_video_codec_h265std_decode.h
│   │   │   │   ├── vulkan_video_codec_h265std_encode.h
│   │   │   │   ├── vulkan_video_codec_h265std.h
│   │   │   │   ├── vulkan_video_codec_vp9std_decode.h
│   │   │   │   ├── vulkan_video_codec_vp9std.h
│   │   │   │   └── vulkan_video_codecs_common.h
│   │   │   └── vulkan/
│   │   │       ├── vk_icd.h
│   │   │       ├── vk_layer.h
│   │   │       ├── vk_platform.h
│   │   │       ├── vulkan_android.h
│   │   │       ├── vulkan_beta.h
│   │   │       ├── vulkan_core.h
│   │   │       ├── vulkan_directfb.h
│   │   │       ├── vulkan_fuchsia.h
│   │   │       ├── vulkan_ggp.h
│   │   │       ├── vulkan_ios.h
│   │   │       ├── vulkan_macos.h
│   │   │       ├── vulkan_metal.h
│   │   │       ├── vulkan_ohos.h
│   │   │       ├── vulkan_screen.h
│   │   │       ├── vulkan_ubm.h
│   │   │       ├── vulkan_vi.h
│   │   │       ├── vulkan_wayland.h
│   │   │       ├── vulkan_win32.h
│   │   │       ├── vulkan_xcb.h
│   │   │       ├── vulkan_xlib_xrandr.h
│   │   │       ├── vulkan_xlib.h
│   │   │       └── vulkan.h
│   │   ├── kmsdrm/
│   │   │   ├── SDL_kmsdrmdyn.c
│   │   │   ├── SDL_kmsdrmdyn.h
│   │   │   ├── SDL_kmsdrmevents.c
│   │   │   ├── SDL_kmsdrmevents.h
│   │   │   ├── SDL_kmsdrmmouse.c
│   │   │   ├── SDL_kmsdrmmouse.h
│   │   │   ├── SDL_kmsdrmopengles.c
│   │   │   ├── SDL_kmsdrmopengles.h
│   │   │   ├── SDL_kmsdrmsym.h
│   │   │   ├── SDL_kmsdrmvideo.c
│   │   │   ├── SDL_kmsdrmvideo.h
│   │   │   ├── SDL_kmsdrmvulkan.c
│   │   │   └── SDL_kmsdrmvulkan.h
│   │   ├── n3ds/
│   │   │   ├── SDL_n3dsevents_c.h
│   │   │   ├── SDL_n3dsevents.c
│   │   │   ├── SDL_n3dsframebuffer_c.h
│   │   │   ├── SDL_n3dsframebuffer.c
│   │   │   ├── SDL_n3dsswkb.c
│   │   │   ├── SDL_n3dsswkb.h
│   │   │   ├── SDL_n3dstouch.c
│   │   │   ├── SDL_n3dstouch.h
│   │   │   ├── SDL_n3dsvideo.c
│   │   │   └── SDL_n3dsvideo.h
│   │   ├── ngage/
│   │   │   ├── SDL_ngagevideo.c
│   │   │   └── SDL_ngagevideo.h
│   │   ├── offscreen/
│   │   │   ├── SDL_offscreenevents_c.h
│   │   │   ├── SDL_offscreenevents.c
│   │   │   ├── SDL_offscreenframebuffer_c.h
│   │   │   ├── SDL_offscreenframebuffer.c
│   │   │   ├── SDL_offscreenopengles.c
│   │   │   ├── SDL_offscreenopengles.h
│   │   │   ├── SDL_offscreenvideo.c
│   │   │   ├── SDL_offscreenvideo.h
│   │   │   ├── SDL_offscreenvulkan.c
│   │   │   ├── SDL_offscreenvulkan.h
│   │   │   ├── SDL_offscreenwindow.c
│   │   │   └── SDL_offscreenwindow.h
│   │   ├── openharmony/
│   │   │   ├── SDL_openharmonyclipboard.c
│   │   │   ├── SDL_openharmonyclipboard.h
│   │   │   ├── SDL_openharmonyevents.c
│   │   │   ├── SDL_openharmonyevents.h
│   │   │   ├── SDL_openharmonykeyboard.c
│   │   │   ├── SDL_openharmonykeyboard.h
│   │   │   ├── SDL_openharmonymouse.c
│   │   │   ├── SDL_openharmonymouse.h
│   │   │   ├── SDL_openharmonyopengl.c
│   │   │   ├── SDL_openharmonyopengl.h
│   │   │   ├── SDL_openharmonyvideo.c
│   │   │   ├── SDL_openharmonyvideo.h
│   │   │   ├── SDL_openharmonyvulkan.c
│   │   │   ├── SDL_openharmonyvulkan.h
│   │   │   ├── SDL_openharmonywindow.c
│   │   │   └── SDL_openharmonywindow.h
│   │   ├── openvr/
│   │   │   ├── openvr_capi.h
│   │   │   ├── SDL_openvrvideo.c
│   │   │   └── SDL_openvrvideo.h
│   │   ├── ps2/
│   │   │   ├── SDL_ps2video.c
│   │   │   └── SDL_ps2video.h
│   │   ├── psp/
│   │   │   ├── SDL_pspevents_c.h
│   │   │   ├── SDL_pspevents.c
│   │   │   ├── SDL_pspgl_c.h
│   │   │   ├── SDL_pspgl.c
│   │   │   ├── SDL_pspvideo.c
│   │   │   └── SDL_pspvideo.h
│   │   ├── qnx/
│   │   │   ├── SDL_qnx.h
│   │   │   ├── SDL_qnxgl.c
│   │   │   ├── SDL_qnxkeyboard.c
│   │   │   ├── SDL_qnxmodes.c
│   │   │   ├── SDL_qnxmouse.c
│   │   │   ├── SDL_qnxpointer.c
│   │   │   └── SDL_qnxvideo.c
│   │   ├── raspberry/
│   │   │   ├── SDL_rpievents_c.h
│   │   │   ├── SDL_rpievents.c
│   │   │   ├── SDL_rpimouse.c
│   │   │   ├── SDL_rpimouse.h
│   │   │   ├── SDL_rpiopengles.c
│   │   │   ├── SDL_rpiopengles.h
│   │   │   ├── SDL_rpivideo.c
│   │   │   └── SDL_rpivideo.h
│   │   ├── riscos/
│   │   │   ├── scancodes_riscos.h
│   │   │   ├── SDL_riscosdefs.h
│   │   │   ├── SDL_riscosevents_c.h
│   │   │   ├── SDL_riscosevents.c
│   │   │   ├── SDL_riscosframebuffer_c.h
│   │   │   ├── SDL_riscosframebuffer.c
│   │   │   ├── SDL_riscosmessagebox.c
│   │   │   ├── SDL_riscosmessagebox.h
│   │   │   ├── SDL_riscosmodes.c
│   │   │   ├── SDL_riscosmodes.h
│   │   │   ├── SDL_riscosmouse.c
│   │   │   ├── SDL_riscosmouse.h
│   │   │   ├── SDL_riscosvideo.c
│   │   │   ├── SDL_riscosvideo.h
│   │   │   ├── SDL_riscoswindow.c
│   │   │   └── SDL_riscoswindow.h
│   │   ├── uikit/
│   │   │   ├── SDL_CurvedContentHosting.swift
│   │   │   ├── SDL_CurvedContentView.swift
│   │   │   ├── SDL_CurvedUIShader.swift
│   │   │   ├── SDL_RealityKitHelper.swift
│   │   │   ├── SDL_uikitappdelegate.h
│   │   │   ├── SDL_uikitappdelegate.m
│   │   │   ├── SDL_UIKitBridge-objc.h
│   │   │   ├── SDL_UIKitBridge-swift.h
│   │   │   ├── SDL_UIKitBridge.m
│   │   │   ├── SDL_uikitclipboard.h
│   │   │   ├── SDL_uikitclipboard.m
│   │   │   ├── SDL_uikitevents.h
│   │   │   ├── SDL_uikitevents.m
│   │   │   ├── SDL_uikitmessagebox.h
│   │   │   ├── SDL_uikitmessagebox.m
│   │   │   ├── SDL_uikitmetalview.h
│   │   │   ├── SDL_uikitmetalview.m
│   │   │   ├── SDL_uikitmodes.h
│   │   │   ├── SDL_uikitmodes.m
│   │   │   ├── SDL_uikitopengles.h
│   │   │   ├── SDL_uikitopengles.m
│   │   │   ├── SDL_uikitopenglview.h
│   │   │   ├── SDL_uikitopenglview.m
│   │   │   ├── SDL_uikitpen.h
│   │   │   ├── SDL_uikitpen.m
│   │   │   ├── SDL_uikitvideo.h
│   │   │   ├── SDL_uikitvideo.m
│   │   │   ├── SDL_uikitview.h
│   │   │   ├── SDL_uikitview.m
│   │   │   ├── SDL_uikitviewcontroller.h
│   │   │   ├── SDL_uikitviewcontroller.m
│   │   │   ├── SDL_uikitviewcontroller.swift
│   │   │   ├── SDL_uikitvulkan.h
│   │   │   ├── SDL_uikitvulkan.m
│   │   │   ├── SDL_uikitwindow.h
│   │   │   └── SDL_uikitwindow.m
│   │   ├── vita/
│   │   │   ├── SDL_vitaframebuffer.c
│   │   │   ├── SDL_vitaframebuffer.h
│   │   │   ├── SDL_vitagl_pvr.c
│   │   │   ├── SDL_vitagles_pvr.c
│   │   │   ├── SDL_vitagles.c
│   │   │   ├── SDL_vitakeyboard.c
│   │   │   ├── SDL_vitakeyboard.h
│   │   │   ├── SDL_vitamessagebox.c
│   │   │   ├── SDL_vitamessagebox.h
│   │   │   ├── SDL_vitamouse_c.h
│   │   │   ├── SDL_vitamouse.c
│   │   │   ├── SDL_vitatouch.c
│   │   │   ├── SDL_vitatouch.h
│   │   │   ├── SDL_vitavideo.c
│   │   │   └── SDL_vitavideo.h
│   │   ├── vivante/
│   │   │   ├── SDL_vivanteopengles.c
│   │   │   ├── SDL_vivanteopengles.h
│   │   │   ├── SDL_vivanteplatform.c
│   │   │   ├── SDL_vivanteplatform.h
│   │   │   ├── SDL_vivantevideo.c
│   │   │   ├── SDL_vivantevideo.h
│   │   │   ├── SDL_vivantevulkan.c
│   │   │   └── SDL_vivantevulkan.h
│   │   ├── wayland/
│   │   │   ├── SDL_waylandclipboard.c
│   │   │   ├── SDL_waylandclipboard.h
│   │   │   ├── SDL_waylandcolor.c
│   │   │   ├── SDL_waylandcolor.h
│   │   │   ├── SDL_waylanddatamanager.c
│   │   │   ├── SDL_waylanddatamanager.h
│   │   │   ├── SDL_waylanddyn.c
│   │   │   ├── SDL_waylanddyn.h
│   │   │   ├── SDL_waylandevents_c.h
│   │   │   ├── SDL_waylandevents.c
│   │   │   ├── SDL_waylandeventthread.c
│   │   │   ├── SDL_waylandeventthread.h
│   │   │   ├── SDL_waylandkeyboard.c
│   │   │   ├── SDL_waylandkeyboard.h
│   │   │   ├── SDL_waylandmessagebox.c
│   │   │   ├── SDL_waylandmessagebox.h
│   │   │   ├── SDL_waylandmouse.c
│   │   │   ├── SDL_waylandmouse.h
│   │   │   ├── SDL_waylandopengles.c
│   │   │   ├── SDL_waylandopengles.h
│   │   │   ├── SDL_waylandshmbuffer.c
│   │   │   ├── SDL_waylandshmbuffer.h
│   │   │   ├── SDL_waylandsym.h
│   │   │   ├── SDL_waylandutil.c
│   │   │   ├── SDL_waylandutil.h
│   │   │   ├── SDL_waylandvideo.c
│   │   │   ├── SDL_waylandvideo.h
│   │   │   ├── SDL_waylandvulkan.c
│   │   │   ├── SDL_waylandvulkan.h
│   │   │   ├── SDL_waylandwindow.c
│   │   │   └── SDL_waylandwindow.h
│   │   ├── windows/
│   │   │   ├── SDL_msctf.h
│   │   │   ├── SDL_windowsclipboard.c
│   │   │   ├── SDL_windowsclipboard.h
│   │   │   ├── SDL_windowsevents.c
│   │   │   ├── SDL_windowsevents.h
│   │   │   ├── SDL_windowsframebuffer.c
│   │   │   ├── SDL_windowsframebuffer.h
│   │   │   ├── SDL_windowsgameinput.cpp
│   │   │   ├── SDL_windowsgameinput.h
│   │   │   ├── SDL_windowskeyboard.c
│   │   │   ├── SDL_windowskeyboard.h
│   │   │   ├── SDL_windowsmessagebox.c
│   │   │   ├── SDL_windowsmessagebox.h
│   │   │   ├── SDL_windowsmodes.c
│   │   │   ├── SDL_windowsmodes.h
│   │   │   ├── SDL_windowsmouse.c
│   │   │   ├── SDL_windowsmouse.h
│   │   │   ├── SDL_windowsopengl.c
│   │   │   ├── SDL_windowsopengl.h
│   │   │   ├── SDL_windowsopengles.c
│   │   │   ├── SDL_windowsopengles.h
│   │   │   ├── SDL_windowsrawinput.c
│   │   │   ├── SDL_windowsrawinput.h
│   │   │   ├── SDL_windowsshape.c
│   │   │   ├── SDL_windowsshape.h
│   │   │   ├── SDL_windowsvideo.c
│   │   │   ├── SDL_windowsvideo.h
│   │   │   ├── SDL_windowsvulkan.c
│   │   │   ├── SDL_windowsvulkan.h
│   │   │   ├── SDL_windowswindow.c
│   │   │   ├── SDL_windowswindow.h
│   │   │   └── wmmsg.h
│   │   ├── x11/
│   │   │   ├── edid-parse.c
│   │   │   ├── edid.h
│   │   │   ├── SDL_x11clipboard.c
│   │   │   ├── SDL_x11clipboard.h
│   │   │   ├── SDL_x11dyn.c
│   │   │   ├── SDL_x11dyn.h
│   │   │   ├── SDL_x11events.c
│   │   │   ├── SDL_x11events.h
│   │   │   ├── SDL_x11framebuffer.c
│   │   │   ├── SDL_x11framebuffer.h
│   │   │   ├── SDL_x11keyboard.c
│   │   │   ├── SDL_x11keyboard.h
│   │   │   ├── SDL_x11messagebox.c
│   │   │   ├── SDL_x11messagebox.h
│   │   │   ├── SDL_x11modes.c
│   │   │   ├── SDL_x11modes.h
│   │   │   ├── SDL_x11mouse.c
│   │   │   ├── SDL_x11mouse.h
│   │   │   ├── SDL_x11opengl.c
│   │   │   ├── SDL_x11opengl.h
│   │   │   ├── SDL_x11opengles.c
│   │   │   ├── SDL_x11opengles.h
│   │   │   ├── SDL_x11pen.c
│   │   │   ├── SDL_x11pen.h
│   │   │   ├── SDL_x11settings.c
│   │   │   ├── SDL_x11settings.h
│   │   │   ├── SDL_x11shape.c
│   │   │   ├── SDL_x11shape.h
│   │   │   ├── SDL_x11sym.h
│   │   │   ├── SDL_x11toolkit.c
│   │   │   ├── SDL_x11toolkit.h
│   │   │   ├── SDL_x11touch.c
│   │   │   ├── SDL_x11touch.h
│   │   │   ├── SDL_x11video.c
│   │   │   ├── SDL_x11video.h
│   │   │   ├── SDL_x11vulkan.c
│   │   │   ├── SDL_x11vulkan.h
│   │   │   ├── SDL_x11window.c
│   │   │   ├── SDL_x11window.h
│   │   │   ├── SDL_x11xfixes.c
│   │   │   ├── SDL_x11xfixes.h
│   │   │   ├── SDL_x11xinput2.c
│   │   │   ├── SDL_x11xinput2.h
│   │   │   ├── SDL_x11xsync.c
│   │   │   ├── SDL_x11xsync.h
│   │   │   ├── SDL_x11xtest.c
│   │   │   ├── SDL_x11xtest.h
│   │   │   ├── xsettings-client.c
│   │   │   └── xsettings-client.h
│   │   ├── yuv2rgb/
│   │   │   ├── LICENSE
│   │   │   ├── README.md
│   │   │   ├── yuv_rgb_common.h
│   │   │   ├── yuv_rgb_internal.h
│   │   │   ├── yuv_rgb_lsx_func.h
│   │   │   ├── yuv_rgb_lsx.c
│   │   │   ├── yuv_rgb_lsx.h
│   │   │   ├── yuv_rgb_sse_func.h
│   │   │   ├── yuv_rgb_sse.c
│   │   │   ├── yuv_rgb_sse.h
│   │   │   ├── yuv_rgb_std_func.h
│   │   │   ├── yuv_rgb_std.c
│   │   │   ├── yuv_rgb_std.h
│   │   │   └── yuv_rgb.h
│   │   ├── miniz.h
│   │   ├── SDL_blit_0.c
│   │   ├── SDL_blit_1.c
│   │   ├── SDL_blit_A.c
│   │   ├── SDL_blit_auto.c
│   │   ├── SDL_blit_auto.h
│   │   ├── SDL_blit_copy.c
│   │   ├── SDL_blit_copy.h
│   │   ├── SDL_blit_N.c
│   │   ├── SDL_blit_slow.c
│   │   ├── SDL_blit_slow.h
│   │   ├── SDL_blit.c
│   │   ├── SDL_blit.h
│   │   ├── SDL_bmp.c
│   │   ├── SDL_clipboard_c.h
│   │   ├── SDL_clipboard.c
│   │   ├── SDL_cursor_names.c
│   │   ├── SDL_egl_c.h
│   │   ├── SDL_egl.c
│   │   ├── SDL_fillrect.c
│   │   ├── SDL_pixels_c.h
│   │   ├── SDL_pixels.c
│   │   ├── SDL_rect_c.h
│   │   ├── SDL_rect_impl.h
│   │   ├── SDL_rect.c
│   │   ├── SDL_RLEaccel_c.h
│   │   ├── SDL_RLEaccel.c
│   │   ├── SDL_rotate.c
│   │   ├── SDL_rotate.h
│   │   ├── SDL_stb_c.h
│   │   ├── SDL_stb.c
│   │   ├── SDL_stretch.c
│   │   ├── SDL_surface_c.h
│   │   ├── SDL_surface.c
│   │   ├── SDL_sysvideo.h
│   │   ├── SDL_video_c.h
│   │   ├── SDL_video_displays.c
│   │   ├── SDL_video_gl.c
│   │   ├── SDL_video_modes.c
│   │   ├── SDL_video_textinput.c
│   │   ├── SDL_video_unsupported.c
│   │   ├── SDL_video_unsupported.h
│   │   ├── SDL_video_vulkan.c
│   │   ├── SDL_video_windowtexture.c
│   │   ├── SDL_video.c
│   │   ├── SDL_vulkan_internal.h
│   │   ├── SDL_vulkan_utils.c
│   │   ├── SDL_yuv_c.h
│   │   ├── SDL_yuv.c
│   │   ├── sdlgenblit.pl
│   │   └── stb_image.h
│   ├── SDL_assert_c.h
│   ├── SDL_assert.c
│   ├── SDL_error_c.h
│   ├── SDL_error.c
│   ├── SDL_generic_hashtable.h
│   ├── SDL_guid.c
│   ├── SDL_hashtable.c
│   ├── SDL_hashtable.h
│   ├── SDL_hints_c.h
│   ├── SDL_hints.c
│   ├── SDL_internal.h
│   ├── SDL_list.c
│   ├── SDL_list.h
│   ├── SDL_log_c.h
│   ├── SDL_log.c
│   ├── SDL_menu.h
│   ├── SDL_properties_c.h
│   ├── SDL_properties.c
│   ├── SDL_utils_c.h
│   ├── SDL_utils.c
│   └── SDL.c
├── test/
│   ├── android/
│   │   ├── cmake/
│   │   │   ├── res/
│   │   │   │   ├── values/
│   │   │   │   │   └── strings.xml.cmake
│   │   │   │   └── xml/
│   │   │   │       └── shortcuts.xml.cmake
│   │   │   ├── AndroidManifest.xml.cmake
│   │   │   ├── AndroidManifest.xr.xml.cmake
│   │   │   ├── SDLEntryTestActivity.java.cmake
│   │   │   └── SDLTestActivity.java.cmake
│   │   └── res/
│   │       ├── drawable/
│   │       │   └── sdl-test_foreground.xml
│   │       ├── layout/
│   │       │   └── arguments_layout.xml
│   │       ├── mipmap-anydpi-v26/
│   │       │   ├── sdl-test_round.xml
│   │       │   └── sdl-test.xml
│   │       ├── mipmap-hdpi/
│   │       │   ├── sdl-test_round.png
│   │       │   └── sdl-test.png
│   │       ├── mipmap-mdpi/
│   │       │   ├── sdl-test_round.png
│   │       │   └── sdl-test.png
│   │       ├── mipmap-xhdpi/
│   │       │   ├── sdl-test_round.png
│   │       │   └── sdl-test.png
│   │       ├── mipmap-xxhdpi/
│   │       │   ├── sdl-test_round.png
│   │       │   └── sdl-test.png
│   │       ├── mipmap-xxxhdpi/
│   │       │   ├── sdl-test_round.png
│   │       │   └── sdl-test.png
│   │       └── values/
│   │           ├── arg_strings.xml
│   │           ├── sdl-test_background.xml
│   │           └── styles.xml
│   ├── emscripten/
│   │   ├── driver.py
│   │   ├── joystick-pre.js
│   │   ├── pre.js
│   │   └── server.py
│   ├── n3ds/
│   │   └── logo48x48.png
│   ├── testgpu/
│   │   ├── build-shaders.sh
│   │   ├── cube.frag.dxil.h
│   │   ├── cube.frag.hlsl
│   │   ├── cube.frag.msl.h
│   │   ├── cube.frag.spv.h
│   │   ├── cube.hlsli
│   │   ├── cube.vert.dxil.h
│   │   ├── cube.vert.hlsl
│   │   ├── cube.vert.msl.h
│   │   ├── cube.vert.spv.h
│   │   ├── overlay.frag.dxil.h
│   │   ├── overlay.frag.hlsl
│   │   ├── overlay.frag.msl.h
│   │   ├── overlay.frag.spv.h
│   │   ├── overlay.hlsli
│   │   ├── overlay.vert.dxil.h
│   │   ├── overlay.vert.hlsl
│   │   ├── overlay.vert.msl.h
│   │   └── overlay.vert.spv.h
│   ├── unix/
│   │   └── gdbcmds.txt
│   ├── win32/
│   │   └── sdlprocdump.c
│   ├── audiofile.png
│   ├── build-shaders.sh
│   ├── checkkeys.c
│   ├── childprocess.c
│   ├── CMakeLists.txt
│   ├── COPYING
│   ├── gamepad_axis_arrow.h
│   ├── gamepad_axis_arrow.png
│   ├── gamepad_axis.h
│   ├── gamepad_axis.png
│   ├── gamepad_back.h
│   ├── gamepad_back.png
│   ├── gamepad_battery_unknown.h
│   ├── gamepad_battery_unknown.png
│   ├── gamepad_battery_wired.h
│   ├── gamepad_battery_wired.png
│   ├── gamepad_battery.h
│   ├── gamepad_battery.png
│   ├── gamepad_button_background.h
│   ├── gamepad_button_background.png
│   ├── gamepad_button_small.h
│   ├── gamepad_button_small.png
│   ├── gamepad_button.h
│   ├── gamepad_button.png
│   ├── gamepad_dual_touchpad.h
│   ├── gamepad_dual_touchpad.png
│   ├── gamepad_face_abxy.h
│   ├── gamepad_face_abxy.png
│   ├── gamepad_face_axby.h
│   ├── gamepad_face_axby.png
│   ├── gamepad_face_bayx.h
│   ├── gamepad_face_bayx.png
│   ├── gamepad_face_sony.h
│   ├── gamepad_face_sony.png
│   ├── gamepad_front.h
│   ├── gamepad_front.png
│   ├── gamepad_grip_sense.h
│   ├── gamepad_grip_sense.png
│   ├── gamepad_touchpad.h
│   ├── gamepad_touchpad.png
│   ├── gamepad_wired.h
│   ├── gamepad_wired.png
│   ├── gamepad_wireless.h
│   ├── gamepad_wireless.png
│   ├── gamepadutils.c
│   ├── gamepadutils.h
│   ├── glass.h
│   ├── glass.png
│   ├── icon.h
│   ├── icon.png
│   ├── icon2x.png
│   ├── LICENSE.txt
│   ├── logaudiodev.png
│   ├── loopwave.c
│   ├── main.cpp
│   ├── moose.dat
│   ├── msdf_font.csv
│   ├── msdf_font.png
│   ├── physaudiodev.png
│   ├── picture.xbm
│   ├── pretest.c
│   ├── relative_mode.markdown
│   ├── sample.png
│   ├── sample.wav
│   ├── sdl-test_round.png
│   ├── soundboard_levels.png
│   ├── soundboard.png
│   ├── speaker.png
│   ├── sword.wav
│   ├── template.test.in
│   ├── testasyncio.c
│   ├── testatomic.c
│   ├── testaudio.c
│   ├── testaudiohotplug.c
│   ├── testaudioinfo.c
│   ├── testaudiorecording.c
│   ├── testaudiostreamdynamicresample.c
│   ├── testautomation_audio.c
│   ├── testautomation_blit.c
│   ├── testautomation_clipboard.c
│   ├── testautomation_events.c
│   ├── testautomation_guid.c
│   ├── testautomation_hints.c
│   ├── testautomation_images.c
│   ├── testautomation_images.h
│   ├── testautomation_intrinsics.c
│   ├── testautomation_iostream.c
│   ├── testautomation_joystick.c
│   ├── testautomation_keyboard.c
│   ├── testautomation_log.c
│   ├── testautomation_main.c
│   ├── testautomation_math.c
│   ├── testautomation_mouse.c
│   ├── testautomation_pixels.c
│   ├── testautomation_platform.c
│   ├── testautomation_properties.c
│   ├── testautomation_rect.c
│   ├── testautomation_render.c
│   ├── testautomation_sdltest.c
│   ├── testautomation_stdlib.c
│   ├── testautomation_subsystems.c
│   ├── testautomation_suites.h
│   ├── testautomation_surface.c
│   ├── testautomation_time.c
│   ├── testautomation_timer.c
│   ├── testautomation_video.c
│   ├── testautomation.c
│   ├── testbounds.c
│   ├── testcamera.c
│   ├── testclipboard.c
│   ├── testcolorspace.c
│   ├── testcontroller.c
│   ├── testcustomcursor.c
│   ├── testdescriptor.c
│   ├── testdialog.c
│   ├── testdisplayinfo.c
│   ├── testdlopennote.c
│   ├── testdraw.c
│   ├── testdrawchessboard.c
│   ├── testdropfile.c
│   ├── testdynaudioreopen.c
│   ├── testerror.c
│   ├── testevdev.c
│   ├── testffmpeg_vulkan.c
│   ├── testffmpeg_vulkan.h
│   ├── testffmpeg.c
│   ├── testfile.c
│   ├── testfilesystem.c
│   ├── testgeometry.c
│   ├── testgl.c
│   ├── testgles.c
│   ├── testgles2.c
│   ├── testgpu_simple_clear.c
│   ├── testgpu_spinning_cube_xr.c
│   ├── testgpu_spinning_cube.c
│   ├── testgpurender_effects_CRT.frag.dxil.h
│   ├── testgpurender_effects_CRT.frag.hlsl
│   ├── testgpurender_effects_CRT.frag.msl.h
│   ├── testgpurender_effects_CRT.frag.spv.h
│   ├── testgpurender_effects_grayscale.frag.dxil.h
│   ├── testgpurender_effects_grayscale.frag.hlsl
│   ├── testgpurender_effects_grayscale.frag.msl.h
│   ├── testgpurender_effects_grayscale.frag.spv.h
│   ├── testgpurender_effects.c
│   ├── testgpurender_msdf.c
│   ├── testgpurender_msdf.frag.dxil.h
│   ├── testgpurender_msdf.frag.hlsl
│   ├── testgpurender_msdf.frag.msl.h
│   ├── testgpurender_msdf.frag.spv.h
│   ├── testhaptic.c
│   ├── testhittesting.c
│   ├── testhotplug.c
│   ├── testiconv.c
│   ├── testime.c
│   ├── testintersections.c
│   ├── testkeys.c
│   ├── testloadso.c
│   ├── testlocale.c
│   ├── testlock.c
│   ├── testmanymouse.c
│   ├── testmessage.c
│   ├── testmodal.c
│   ├── testmouse.c
│   ├── testmultiaudio.c
│   ├── testnative.c
│   ├── testnative.h
│   ├── testnativecocoa.m
│   ├── testnativew32.c
│   ├── testnativewayland.c
│   ├── testnativex11.c
│   ├── testnotification.c
│   ├── testoffscreen.c
│   ├── testoverlay.c
│   ├── testpalette.c
│   ├── testpen.c
│   ├── testplatform.c
│   ├── testpopup.c
│   ├── testpower.c
│   ├── testprocess.c
│   ├── testqsort.c
│   ├── testrelative.c
│   ├── testrendercopyex.c
│   ├── testrendertarget.c
│   ├── testresample.c
│   ├── testrotate.c
│   ├── testrumble.c
│   ├── testrwlock.c
│   ├── testscale.c
│   ├── testsem.c
│   ├── testsensor.c
│   ├── testshader.c
│   ├── testshape.c
│   ├── testsoftwaretransparent.c
│   ├── testsprite.c
│   ├── testsprite.cpp
│   ├── testspriteminimal.c
│   ├── testspritesurface.c
│   ├── testsurround.c
│   ├── testsymbols.c
│   ├── testthread.c
│   ├── testtime.c
│   ├── testtimer.c
│   ├── testtray.c
│   ├── testurl.c
│   ├── testutils.c
│   ├── testutils.h
│   ├── testver.c
│   ├── testviewport.c
│   ├── testvulkan.c
│   ├── testwaylandcustom.c
│   ├── testwm.c
│   ├── testyuv_cvt.c
│   ├── testyuv_cvt.h
│   ├── testyuv.c
│   ├── testyuv.png
│   ├── torturethread.c
│   ├── trashcan.png
│   ├── unifont-15.1.05-license.txt
│   ├── unifont-15.1.05.hex
│   └── utf8.txt
├── VisualC/
│   ├── examples/
│   │   ├── asyncio/
│   │   │   └── 01-load-bitmaps/
│   │   │       └── 01-load-bitmaps.vcxproj
│   │   ├── audio/
│   │   │   ├── 01-simple-playback/
│   │   │   │   └── 01-simple-playback.vcxproj
│   │   │   ├── 02-simple-playback-callback/
│   │   │   │   └── 02-simple-playback-callback.vcxproj
│   │   │   ├── 03-load-wav/
│   │   │   │   └── 03-load-wav.vcxproj
│   │   │   ├── 04-multiple-streams/
│   │   │   │   └── 04-multiple-streams.vcxproj
│   │   │   └── 05-planar-data/
│   │   │       └── 05-planar-data.vcxproj
│   │   ├── camera/
│   │   │   └── 01-read-and-draw/
│   │   │       └── 01-read-and-draw.vcxproj
│   │   ├── demo/
│   │   │   ├── 01-snake/
│   │   │   │   └── 01-snake.vcxproj
│   │   │   ├── 02-woodeneye-008/
│   │   │   │   └── 02-woodeneye-008.vcxproj
│   │   │   ├── 03-infinite-monkeys/
│   │   │   │   └── 03-infinite-monkeys.vcxproj
│   │   │   └── 04-bytepusher/
│   │   │       └── 04-bytepusher.vcxproj
│   │   ├── input/
│   │   │   ├── 01-joystick-polling/
│   │   │   │   └── 01-joystick-polling.vcxproj
│   │   │   ├── 02-joystick-events/
│   │   │   │   └── 02-joystick-events.vcxproj
│   │   │   ├── 03-gamepad-polling/
│   │   │   │   └── 03-gamepad-polling.vcxproj
│   │   │   ├── 04-gamepad-events/
│   │   │   │   └── 04-gamepad-events.vcxproj
│   │   │   └── 05-gamepad-rumble/
│   │   │       └── 05-gamepad-rumble.vcxproj
│   │   ├── misc/
│   │   │   ├── 01-power/
│   │   │   │   └── 01-power.vcxproj
│   │   │   ├── 02-clipboard/
│   │   │   │   └── 02-clipboard.vcxproj
│   │   │   └── 03-locale/
│   │   │       └── 03-locale.vcxproj
│   │   ├── pen/
│   │   │   └── 01-drawing-lines/
│   │   │       └── 01-drawing-lines.vcxproj
│   │   ├── renderer/
│   │   │   ├── 01-clear/
│   │   │   │   └── 01-clear.vcxproj
│   │   │   ├── 02-primitives/
│   │   │   │   └── 02-primitives.vcxproj
│   │   │   ├── 03-lines/
│   │   │   │   └── 03-lines.vcxproj
│   │   │   ├── 04-points/
│   │   │   │   └── 04-points.vcxproj
│   │   │   ├── 05-rectangles/
│   │   │   │   └── 05-rectangles.vcxproj
│   │   │   ├── 06-textures/
│   │   │   │   └── 06-textures.vcxproj
│   │   │   ├── 07-streaming-textures/
│   │   │   │   └── 07-streaming-textures.vcxproj
│   │   │   ├── 08-rotating-textures/
│   │   │   │   └── 08-rotating-textures.vcxproj
│   │   │   ├── 09-scaling-textures/
│   │   │   │   └── 09-scaling-textures.vcxproj
│   │   │   ├── 10-geometry/
│   │   │   │   └── 10-geometry.vcxproj
│   │   │   ├── 11-color-mods/
│   │   │   │   └── 11-color-mods.vcxproj
│   │   │   ├── 14-viewport/
│   │   │   │   └── 14-viewport.vcxproj
│   │   │   ├── 15-cliprect/
│   │   │   │   └── 15-cliprect.vcxproj
│   │   │   ├── 17-read-pixels/
│   │   │   │   └── 17-read-pixels.vcxproj
│   │   │   ├── 18-debug-text/
│   │   │   │   └── 18-debug-text.vcxproj
│   │   │   ├── 19-affine-textures/
│   │   │   │   └── 19-affine-textures.vcxproj
│   │   │   └── 20-blending/
│   │   │       └── 20-blending.vcxproj
│   │   ├── storage/
│   │   │   └── 01-user/
│   │   │       └── 01-user.vcxproj
│   │   ├── Examples.props
│   │   └── generate.py
│   ├── SDL/
│   │   ├── Directory.Build.props
│   │   ├── SDL.vcxproj
│   │   └── SDL.vcxproj.filters
│   ├── SDL_test/
│   │   ├── Directory.Build.props
│   │   ├── SDL_test.vcxproj
│   │   └── SDL_test.vcxproj.filters
│   ├── tests/
│   │   ├── checkkeys/
│   │   │   └── checkkeys.vcxproj
│   │   ├── loopwave/
│   │   │   └── loopwave.vcxproj
│   │   ├── testatomic/
│   │   │   └── testatomic.vcxproj
│   │   ├── testautomation/
│   │   │   └── testautomation.vcxproj
│   │   ├── testcontroller/
│   │   │   └── testcontroller.vcxproj
│   │   ├── testdialog/
│   │   │   └── testdialog.vcxproj
│   │   ├── testdraw/
│   │   │   └── testdraw.vcxproj
│   │   ├── testfile/
│   │   │   └── testfile.vcxproj
│   │   ├── testgl/
│   │   │   └── testgl.vcxproj
│   │   ├── testgles2/
│   │   │   └── testgles2.vcxproj
│   │   ├── testoverlay/
│   │   │   └── testoverlay.vcxproj
│   │   ├── testpen/
│   │   │   └── testpen.vcxproj
│   │   ├── testplatform/
│   │   │   └── testplatform.vcxproj
│   │   ├── testpower/
│   │   │   └── testpower.vcxproj
│   │   ├── testrendertarget/
│   │   │   └── testrendertarget.vcxproj
│   │   ├── testrumble/
│   │   │   └── testrumble.vcxproj
│   │   ├── testscale/
│   │   │   └── testscale.vcxproj
│   │   ├── testsensor/
│   │   │   └── testsensor.vcxproj
│   │   ├── testshape/
│   │   │   └── testshape.vcxproj
│   │   ├── testsoftwaretransparent/
│   │   │   └── testsoftwaretransparent.vcxproj
│   │   ├── testsprite/
│   │   │   └── testsprite.vcxproj
│   │   ├── testsurround/
│   │   │   └── testsurround.vcxproj
│   │   ├── testtray/
│   │   │   └── testtray.vcxproj
│   │   ├── testvulkan/
│   │   │   └── testvulkan.vcxproj
│   │   ├── testwm/
│   │   │   └── testwm.vcxproj
│   │   └── testyuv/
│   │       └── testyuv.vcxproj
│   ├── clean.sh
│   └── SDL.sln
├── VisualC-GDK/
│   ├── logos/
│   │   ├── Logo100x100.png
│   │   ├── Logo150x150.png
│   │   ├── Logo44x44.png
│   │   ├── Logo480x480.png
│   │   └── SplashScreenImage.png
│   ├── SDL/
│   │   ├── SDL.vcxproj
│   │   └── SDL.vcxproj.filters
│   ├── SDL_test/
│   │   └── SDL_test.vcxproj
│   ├── tests/
│   │   ├── testcontroller/
│   │   │   ├── wingdk/
│   │   │   │   └── MicrosoftGame.config
│   │   │   ├── xboxone/
│   │   │   │   └── MicrosoftGame.config
│   │   │   ├── xboxseries/
│   │   │   │   └── MicrosoftGame.config
│   │   │   ├── PackageLayout.xml
│   │   │   ├── testcontroller.vcxproj
│   │   │   └── testcontroller.vcxproj.filters
│   │   ├── testgdk/
│   │   │   ├── src/
│   │   │   │   └── testgdk.cpp
│   │   │   ├── wingdk/
│   │   │   │   └── MicrosoftGame.config
│   │   │   ├── xboxone/
│   │   │   │   └── MicrosoftGame.config
│   │   │   ├── xboxseries/
│   │   │   │   └── MicrosoftGame.config
│   │   │   ├── PackageLayout.xml
│   │   │   ├── testgdk.vcxproj
│   │   │   └── testgdk.vcxproj.filters
│   │   └── testsprite/
│   │       ├── wingdk/
│   │       │   └── MicrosoftGame.config
│   │       ├── xboxone/
│   │       │   └── MicrosoftGame.config
│   │       ├── xboxseries/
│   │       │   └── MicrosoftGame.config
│   │       ├── PackageLayout.xml
│   │       ├── testsprite.vcxproj
│   │       └── testsprite.vcxproj.filters
│   ├── clean.sh
│   └── SDL.sln
├── wayland-protocols/
│   ├── alpha-modifier-v1.xml
│   ├── color-management-v1.xml
│   ├── cursor-shape-v1.xml
│   ├── fractional-scale-v1.xml
│   ├── frog-color-management-v1.xml
│   ├── idle-inhibit-unstable-v1.xml
│   ├── input-timestamps-unstable-v1.xml
│   ├── keyboard-shortcuts-inhibit-unstable-v1.xml
│   ├── pointer-constraints-unstable-v1.xml
│   ├── pointer-gestures-unstable-v1.xml
│   ├── pointer-warp-v1.xml
│   ├── primary-selection-unstable-v1.xml
│   ├── relative-pointer-unstable-v1.xml
│   ├── single-pixel-buffer-v1.xml
│   ├── tablet-v2.xml
│   ├── text-input-unstable-v3.xml
│   ├── viewporter.xml
│   ├── wayland.xml
│   ├── xdg-activation-v1.xml
│   ├── xdg-decoration-unstable-v1.xml
│   ├── xdg-dialog-v1.xml
│   ├── xdg-foreign-unstable-v2.xml
│   ├── xdg-output-unstable-v1.xml
│   ├── xdg-session-management-v1.xml
│   ├── xdg-shell.xml
│   ├── xdg-toplevel-icon-v1.xml
│   └── xdg-toplevel-tag-v1.xml
├── Xcode/
│   ├── SDL/
│   │   ├── pkg-support/
│   │   │   ├── resources/
│   │   │   │   ├── CMake/
│   │   │   │   │   ├── SDL3Config.cmake
│   │   │   │   │   └── SDL3ConfigVersion.cmake
│   │   │   │   ├── framework/
│   │   │   │   │   └── INSTALL.md
│   │   │   │   ├── INSTALL.md
│   │   │   │   └── SDL_DS_Store
│   │   │   ├── share/
│   │   │   │   └── cmake/
│   │   │   │       └── SDL3/
│   │   │   │           ├── SDL3Config.cmake
│   │   │   │           └── SDL3ConfigVersion.cmake
│   │   │   ├── build.xcconfig
│   │   │   ├── sdl_logo.pdf
│   │   │   └── SDL.info
│   │   ├── SDL.xcodeproj/
│   │   │   └── project.pbxproj
│   │   ├── SDL3/
│   │   │   └── Info.plist
│   │   ├── config.xcconfig
│   │   └── Info-Framework.plist
│   ├── SDLTest/
│   │   ├── SDLTest.xcodeproj/
│   │   │   └── project.pbxproj
│   │   ├── config.xcconfig
│   │   └── test-Info.plist
│   └── XcodeDocSet/
│       └── Doxyfile
├── AGENTS.md
├── Android.mk
├── BUGS.txt
├── CLAUDE.md
├── CMakeLists.txt
├── CREDITS.md
├── INSTALL.md
├── LICENSE.txt
├── README.md
└── WhatsNew.txt
```
