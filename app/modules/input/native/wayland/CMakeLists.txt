file(
  GLOB WAYLAND_SRC
  CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
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
