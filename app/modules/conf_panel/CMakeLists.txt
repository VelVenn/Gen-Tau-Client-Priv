add_subdirectory(element)
add_subdirectory(subpane)

file(
  GLOB QML_FILES
  RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}"
  CONFIGURE_DEPENDS "*.qml"
)

gt_register_qml_mod(
  TARGET conf-panel
  URI_SUFFIX Conf
  REAL_TARGET_VAR CONF_PANEL_TAR
  QML_FILES
    ${QML_FILES}
  QT_ARGS
    STATIC
    DEPENDENCIES
      TARGET Gentau::Qml::Conf::Subpane
)

target_link_libraries(${CONF_PANEL_TAR}
  PRIVATE
    Qt6::Quick
    Gentau::App::ConfPanelSub
)