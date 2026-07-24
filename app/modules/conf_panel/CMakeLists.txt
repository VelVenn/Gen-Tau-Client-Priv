add_subdirectory(element)
add_subdirectory(subpane)

file(
  GLOB QML_FILES
  RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}"
  CONFIGURE_DEPENDS "*.qml"
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
      TARGET Gentau::Qml::Context
      TARGET Gentau::Qml::Service::Conn

      TARGET Gentau::Qml::ConfPanel::Subpane
      TARGET Gentau::Qml::Settings::UiPref
)

target_link_libraries(${CONF_PANEL_TAR}
  PRIVATE
    Qt6::Quick

    Gentau::App::AppContext::Plugin
    Gentau::App::ConfPanelSub
    Gentau::App::SettingsUiPref
)
