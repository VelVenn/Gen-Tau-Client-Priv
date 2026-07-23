pkg_get_variable(WAYLAND_PROTOCOLS_DATA_DIR wayland-protocols pkgdatadir)

if(NOT WAYLAND_PROTOCOLS_DATA_DIR)
  message(FATAL_ERROR "Unable to locate the wayland-protocols data directory")
endif()

file(
  GLOB WAYLAND_SRC
  CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
)

qt_generate_wayland_protocol_client_sources(${INPUT_TAR}
  PRIVATE_CODE
  FILES
    "${WAYLAND_PROTOCOLS_DATA_DIR}/unstable/pointer-constraints/pointer-constraints-unstable-v1.xml"
    "${WAYLAND_PROTOCOLS_DATA_DIR}/unstable/relative-pointer/relative-pointer-unstable-v1.xml"
)

target_sources(${INPUT_TAR}
  PRIVATE
    ${WAYLAND_SRC}
)

target_link_libraries(${INPUT_TAR}
  PRIVATE
    Qt6::WaylandClient
    Wayland::Client
)
