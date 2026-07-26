file(
  GLOB QML_FILES
  RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}"
  CONFIGURE_DEPENDS "*.qml"
)

gt_register_qml_mod(
  TARGET hero-hud-sub
  URI_SUFFIX HeroHud.Subpane
  REAL_TARGET_VAR HERO_HUD_SUB_TAR
  QML_FILES
    ${QML_FILES}
  QT_ARGS
    STATIC
    IMPORTS
      TARGET Gentau::Qml::Foundation
      TARGET Gentau::Qml::CommonElem
)

target_link_libraries(${HERO_HUD_SUB_TAR}
  PRIVATE
    Qt6::Quick
  PUBLIC
    Gentau::App::UiFoundation::Plugin
    Gentau::App::CommonElem::Plugin
)
