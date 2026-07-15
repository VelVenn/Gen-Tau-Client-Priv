file(
  GLOB QML_FILES
  RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}"
  CONFIGURE_DEPENDS "*.qml"
)

gt_register_qml_mod(
  TARGET conf-panel-sub
  URI_SUFFIX Conf.Subpane
  REAL_TARGET_VAR CONF_PANEL_SUB_TAR
  QML_FILES
    ${QML_FILES}
  QT_ARGS
    STATIC
    IMPORTS
      TARGET Gentau::Qml::Foundation
      TARGET Gentau::Qml::Conf::Element
)

target_link_libraries(${CONF_PANEL_SUB_TAR}
  PRIVATE
    Qt6::Quick
  PUBLIC
    Gentau::App::UiFoundation
    Gentau::App::ConfPanelElem
)