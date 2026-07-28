file(
  GLOB ELEM_FILES
  RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}"
  CONFIGURE_DEPENDS "element/*.qml"
)

file(
  GLOB SUB_FILES
  RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}"
  CONFIGURE_DEPENDS "subpane/*.qml" "subpane/*.js"
)

file(
  GLOB SUB_IMG_FILES 
  RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}"
  CONFIGURE_DEPENDS "subpane/images/*"
)

file(
  GLOB HUD_FILES
  RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}"
  CONFIGURE_DEPENDS "*.qml"
)

gt_register_qml_mod(
  TARGET bot-hud-elem
  URI_SUFFIX BotHud.Element
  REAL_TARGET_VAR BOT_HUD_ELEM_TAR
  QML_FILES
    ${ELEM_FILES}
  QT_ARGS
    STATIC
    IMPORTS
      TARGET Gentau::Qml::Foundation
      TARGET Gentau::Qml::BasicWidgets
      TARGET Gentau::Qml::CommonElem
)

target_link_libraries(${BOT_HUD_ELEM_TAR}
  PRIVATE
    Qt6::Quick
  
  PUBLIC
    Gentau::App::UiFoundation
    Gentau::App::BasicWidgets
    Gentau::App::CommonElem
)

gt_register_qml_mod(
  TARGET bot-hud-sub
  URI_SUFFIX BotHud.Subpane
  REAL_TARGET_VAR BOT_HUD_SUB_TAR
  QML_FILES
    ${SUB_FILES}
  QT_ARGS
    STATIC
    IMPORTS
      TARGET Gentau::Qml::BotHud::Element
      TARGET Gentau::Qml::Service::Conn
      TARGET Gentau::Qml::Message
      TARGET Gentau::Qml::Model::GlobalStatus
      TARGET Gentau::Qml::Model::GameStatus
      TARGET Gentau::Qml::Bot::Common
    RESOURCES ${SUB_IMG_FILES}
)

target_link_libraries(${BOT_HUD_SUB_TAR}
  PRIVATE
    Qt6::Quick
  
  PUBLIC
    ${BOT_HUD_ELEM_TAR}
    Gentau::App::ConnService::Plugin
    Gentau::App::Msg::Plugin
    Gentau::App::GlobalStatusModel::Plugin
    Gentau::App::GameStatusModel::Plugin
    Gentau::App::BotsCommon::Plugin
)

gt_register_qml_mod(
  TARGET bot-hud
  URI_SUFFIX BotHud 
  REAL_TARGET_VAR BOT_HUD_TAR
  QML_FILES
    ${HUD_FILES}
  QT_ARGS
    STATIC
    DEPENDENCIES
      TARGET Gentau::Qml::BotHud::Subpane
      TARGET Gentau::Qml::Settings::UiPref
      TARGET Gentau::Qml::Context

      TARGET Gentau::Qml::Bot
      TARGET Gentau::Qml::Bot::Common
      TARGET Gentau::Qml::Bot::Hero

      TARGET Gentau::Qml::HeroHud
)

target_link_libraries(${BOT_HUD_TAR}
  PRIVATE
    Qt6::Quick
    ${BOT_HUD_SUB_TAR}
    Gentau::App::SettingsUiPref
    Gentau::App::AppContext::Plugin

    Gentau::App::BotsModel::Plugin
    Gentau::App::BotsCommon::Plugin
    Gentau::App::BotsHero::Plugin

    Gentau::App::HeroHud::Plugin
)
