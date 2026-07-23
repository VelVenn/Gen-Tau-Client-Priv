include_guard(GLOBAL)

if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
  pkg_get_variable(WAYLAND_PROTOCOLS_DATA_DIR wayland-protocols pkgdatadir)

  if(NOT WAYLAND_PROTOCOLS_DATA_DIR)
    message(FATAL_ERROR "Unable to locate the wayland-protocols data directory")
  endif()

  qt_generate_wayland_protocol_client_sources(${INPUT_TAR}
    PRIVATE_CODE
    FILES
      "${WAYLAND_PROTOCOLS_DATA_DIR}/unstable/pointer-constraints/pointer-constraints-unstable-v1.xml"
      "${WAYLAND_PROTOCOLS_DATA_DIR}/unstable/relative-pointer/relative-pointer-unstable-v1.xml"
  )
endif()