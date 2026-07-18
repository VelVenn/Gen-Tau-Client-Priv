file(
  GLOB SRC_FILES
  RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}"
  CONFIGURE_DEPENDS "*.hpp" "*.cpp"
)

gt_register_qt_lib(
  TARGET mqtt-adapter
  REAL_TARGET_VAR MQTT_TAR 
  QT_ARGS
    STATIC
    ${SRC_FILES}
)

target_link_libraries(${MQTT_TAR}
  PUBLIC
    Qt6::Core 
    Gentau::Comm
)