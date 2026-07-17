add_subdirectory(element)
add_subdirectory(subpane)

file(
  GLOB QML_FILES
  RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}"
  CONFIGURE_DEPENDS "*.qml"
)

set_source_files_properties(
  InAppSetting.qml
  PROPERTIES QT_QML_SINGLETON_TYPE TRUE
)

gt_register_qml_mod(
  TARGET conf-panel
  URI_SUFFIX ConfPanel
  REAL_TARGET_VAR CONF_PANEL_TAR
  QML_FILES
    ${QML_FILES}
  QT_ARGS
    STATIC
    DEPENDENCIES
      TARGET Gentau::Qml::ConfPanel::Subpane
)

target_link_libraries(${CONF_PANEL_TAR}
  PRIVATE
    Qt6::Quick
    Gentau::App::ConfPanelSub
)
