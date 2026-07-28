add_subdirectory(element)
add_subdirectory(subpane)

file(
  GLOB QML_FILES
  RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}"
  CONFIGURE_DEPENDS "*.qml"
)

gt_register_qml_mod(
  TARGET hero-hud
  URI_SUFFIX HeroHud
  REAL_TARGET_VAR HERO_HUD_TAR
  QML_FILES
    ${QML_FILES}
  QT_ARGS
    STATIC
    DEPENDENCIES
      TARGET Gentau::Qml::HeroHud::Subpane

      TARGET Gentau::Qml::Message

      TARGET Gentau::Qml::Settings::UiPref

      TARGET Gentau::Qml::Bot::Hero 
      TARGET Gentau::Qml::Bot::Common
)

target_link_libraries(${HERO_HUD_TAR}
  PUBLIC
    Qt6::Quick

    Gentau::App::BotsCommon::Plugin
    Gentau::App::BotsHero::Plugin

    Gentau::App::Msg::Plugin

    Gentau::App::SettingsUiPref::Plugin

  PRIVATE
    Gentau::App::HeroHudSub::Plugin
)
