file(
  GLOB_RECURSE COMMON_INC_FILES
  RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}"
  CONFIGURE_DEPENDS "inc/*.hpp"
)

file(
  GLOB COMMON_SRC_FILES
  RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}"
  CONFIGURE_DEPENDS "*.cpp"
)

gt_register_qt_lib(
  TARGET input 
  REAL_TARGET_VAR INPUT_TAR
  QT_ARGS
    STATIC
    ${COMMON_SRC_FILES}
    ${COMMON_INC_FILES}
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
  include(cmake/GGenWaylandSource.cmake)
endif()

add_subdirectory(native)
