include_guard(GLOBAL)

if(CMAKE_SYSTEM_NAME STREQUAL "Linux" AND EXISTS "/proc/sys/fs/binfmt_misc/WSLInterop")
  message(STATUS "WSL detected: ignoring /mnt paths for dependency discovery")

  list(PREPEND CMAKE_IGNORE_PREFIX_PATH "/mnt")

  set(CMAKE_FIND_USE_SYSTEM_ENVIRONMENT_PATH FALSE)
  # set(CMAKE_FIND_USE_CMAKE_ENVIRONMENT_PATH FALSE)
endif()

set(GT_QT_COMPONENTS
  Core
  Quick
  Qml
  QmlModels
  QmlWorkerScript
  Protobuf
  ProtobufQuick
)

find_package(Qt6 6.8 REQUIRED COMPONENTS
  ${GT_QT_COMPONENTS}
)

find_package(PkgConfig REQUIRED)

pkg_check_modules(GST REQUIRED IMPORTED_TARGET gstreamer-1.0>=1.26)
pkg_check_modules(GST_VID REQUIRED IMPORTED_TARGET gstreamer-video-1.0>=1.26)
pkg_check_modules(GST_APP REQUIRED IMPORTED_TARGET gstreamer-app-1.0>=1.26)

set(GEN_TAU_WAYLAND_AVAILABLE OFF)

if(CMAKE_SYSTEM_NAME STREQUAL "Linux"
   AND NOT GEN_TAU_WAYLAND STREQUAL "OFF")

  find_package(Qt6 6.8 QUIET COMPONENTS WaylandClient)
  find_package(Wayland QUIET COMPONENTS Client)
  find_package(WaylandScanner QUIET)

  pkg_get_variable(
    WAYLAND_PROTOCOLS_DATA_DIR
    wayland-protocols
    pkgdatadir
  )

  set(POINTER_CONSTRAINTS_PROTOCOL
    "${WAYLAND_PROTOCOLS_DATA_DIR}/unstable/pointer-constraints/pointer-constraints-unstable-v1.xml"
  )
  set(RELATIVE_POINTER_PROTOCOL
    "${WAYLAND_PROTOCOLS_DATA_DIR}/unstable/relative-pointer/relative-pointer-unstable-v1.xml"
  )

  if(TARGET Qt6::WaylandClient
     AND TARGET Wayland::Client
     AND WaylandScanner_FOUND
     AND EXISTS "${POINTER_CONSTRAINTS_PROTOCOL}"
     AND EXISTS "${RELATIVE_POINTER_PROTOCOL}")

    set(GEN_TAU_WAYLAND_AVAILABLE ON)
    message(STATUS "Wayland input support enabled")

  elseif(GEN_TAU_WAYLAND STREQUAL "ON")
    message(FATAL_ERROR
      "Wayland support was explicitly enabled, but its dependencies are incomplete"
    )
  else()
    message(WARNING
      "Wayland dependencies are incomplete; Wayland input support is disabled"
    )
  endif()
endif()

include(FetchContent)

FetchContent_Declare(
  readerwriterqueue
  GIT_REPOSITORY https://github.com/cameron314/readerwriterqueue
  GIT_TAG master # Using released version may trigger CMake deprecation warning (< 3.10)
)
FetchContent_MakeAvailable(readerwriterqueue)

FetchContent_Declare(
  concurrentqueue
  GIT_REPOSITORY https://github.com/cameron314/concurrentqueue.git
  GIT_TAG v1.0.5
)
FetchContent_MakeAvailable(concurrentqueue)

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
