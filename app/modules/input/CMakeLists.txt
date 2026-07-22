file(
  GLOB_RECURSE SRC_FILES
  RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}"
  CONFIGURE_DEPENDS "inc/*.hpp" "*.cpp"
)

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
)

target_link_libraries(${INPUT_TAR}
  PUBLIC
    Qt6::Core 
    Qt6::Quick
    
    Gentau::App::Msg::Api
    Gentau::App::MqttAdapter

    Gentau::Utils
)