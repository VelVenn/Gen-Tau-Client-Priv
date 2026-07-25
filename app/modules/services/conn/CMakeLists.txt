file(
  GLOB_RECURSE SRC_FILES
  RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}"
  CONFIGURE_DEPENDS "inc/*.hpp" "*.cpp"
)

gt_register_qt_lib(
  TARGET conn-service
  NO_ALIAS
  REAL_TARGET_VAR CONN_SERVICE_TAR
  QT_ARGS
    STATIC
    ${SRC_FILES}
)

gt_register_qml_mod(
  TARGET conn-service
  URI_SUFFIX Service.Conn
)

target_include_directories(${CONN_SERVICE_TAR}
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/inc>
)

target_link_libraries(${CONN_SERVICE_TAR}
  PUBLIC
    Qt6::Core 
    Qt6::Qml

    Gentau::ImgTrans
    Gentau::App::MqttAdapter
)