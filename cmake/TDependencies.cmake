include_guard(GLOBAL)

find_package(Qt6 6.8 REQUIRED COMPONENTS
  Quick
  Qml
  QmlModels
  QmlWorkerScript
  Protobuf
  ProtobufQuick
)
find_package(PkgConfig REQUIRED)

pkg_check_modules(GST REQUIRED IMPORTED_TARGET gstreamer-1.0>=1.26)
pkg_check_modules(GST_VID REQUIRED IMPORTED_TARGET gstreamer-video-1.0>=1.26)
pkg_check_modules(GST_APP REQUIRED IMPORTED_TARGET gstreamer-app-1.0>=1.26)

include(FetchContent)

FetchContent_Declare(
  readerwriterqueue
  GIT_REPOSITORY https://github.com/cameron314/readerwriterqueue
  GIT_TAG master # Using released version may trigger CMake deprecation warning (< 3.10)
)
FetchContent_MakeAvailable(readerwriterqueue)

FetchContent_Declare(
  fmt
  GIT_REPOSITORY https://github.com/fmtlib/fmt
  GIT_TAG 12.1.0
)
FetchContent_MakeAvailable(fmt)

FetchContent_Declare(
  sigslot
  GIT_REPOSITORY https://github.com/palacaze/sigslot
  GIT_TAG v1.2.3
)
FetchContent_MakeAvailable(sigslot)

set(SPDLOG_FMT_EXTERNAL ON CACHE BOOL "Use external fmt library" FORCE)
FetchContent_Declare(
  spdlog
  GIT_REPOSITORY https://github.com/gabime/spdlog.git
  GIT_TAG v1.17.0
)
FetchContent_MakeAvailable(spdlog)

FetchContent_Declare(
  paho-mqtt-cpp
  GIT_REPOSITORY https://github.com/eclipse-paho/paho.mqtt.cpp.git
  GIT_TAG v1.6.0
)
set(PAHO_BUILD_STATIC ON CACHE BOOL "Build static library" FORCE)
set(PAHO_WITH_MQTT_C ON CACHE BOOL "Build with the bundled C library" FORCE)
set(PAHO_WITH_SSL OFF CACHE BOOL "Build with SSL support" FORCE)
FetchContent_MakeAvailable(paho-mqtt-cpp)

FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG v1.17.0
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
set(INSTALL_GMOCK OFF CACHE BOOL "" FORCE)
set(BUILD_GMOCK ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

qt_standard_project_setup(REQUIRES 6.8)
