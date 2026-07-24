file(
  GLOB_RECURSE SRC_FILES
  RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}"
  CONFIGURE_DEPENDS "inc/*.hpp" "private/*.hpp" "*.cpp"
)

gt_register_qt_lib(
  TARGET app-context
  NO_ALIAS
  REAL_TARGET_VAR APP_CONTEXT_TAR
  QT_ARGS
    STATIC
    ${SRC_FILES}
)

gt_register_qml_mod(
  TARGET app-context
  URI_SUFFIX Context
)

target_include_directories(${APP_CONTEXT_TAR}
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/inc>
)

target_link_libraries(${APP_CONTEXT_TAR}
  PUBLIC
    Qt6::Core 
    Qt6::Quick
    Qt6::Qml
    
    Gentau::ImgTrans
    Gentau::Utils

    Gentau::App::MqttAdapter
    Gentau::App::Input
)
