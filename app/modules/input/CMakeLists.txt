file(
  GLOB_RECURSE SRC_FILES
  RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}"
  CONFIGURE_DEPENDS "inc/*.hpp" "*.cpp"
)

list(FILTER SRC_FILES EXCLUDE REGEX "^native/wayland/")
if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
  list(APPEND SRC_FILES native/wayland/GWaylandCursorControl.cpp)
endif()

gt_register_qt_lib(
  TARGET input 
  REAL_TARGET_VAR INPUT_TAR
  QT_ARGS
    STATIC
    ${SRC_FILES}
)

target_include_directories(${INPUT_TAR}
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/inc>
  PRIVATE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)

target_link_libraries(${INPUT_TAR}
  PUBLIC
    Qt6::Core 
    Qt6::Quick
    
    Gentau::App::Msg::Api
    Gentau::App::MqttAdapter

    Gentau::Utils
)

if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
  enable_language(C)

  find_package(Qt6 REQUIRED COMPONENTS WaylandClient)
  find_package(Wayland REQUIRED COMPONENTS Client)
  find_package(WaylandScanner REQUIRED)

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

  target_link_libraries(${INPUT_TAR}
    PRIVATE
      Qt6::WaylandClient
      Wayland::Client
  )
endif()
