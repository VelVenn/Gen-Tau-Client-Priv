file(
  GLOB_RECURSE SRC_FILES
  RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}"
  CONFIGURE_DEPENDS "inc/*.hpp" "*.cpp"
)

gt_register_qt_lib(
  TARGET app-context
  REAL_TARGET_VAR APP_CONTEXT_TAR
  QT_ARGS
    STATIC
    ${SRC_FILES}
)

target_include_directories(${APP_CONTEXT_TAR}
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/inc>
)

target_link_libraries(${APP_CONTEXT_TAR}
  PUBLIC
    Qt6::Core 
    Qt6::Quick
    
    Gentau::ImgTrans
    Gentau::Utils

    Gentau::App::MqttAdapter
)
