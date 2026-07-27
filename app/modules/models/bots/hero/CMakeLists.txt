file(
  GLOB_RECURSE SRC_FILES
  RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}"
  CONFIGURE_DEPENDS "inc/*.hpp" "*.cpp"
)

gt_register_qml_mod(
  TARGET bots-hero
  URI_SUFFIX Bot.Hero
  REAL_TARGET_VAR BOTS_HERO_TAR
  QT_ARGS
    STATIC
    SOURCES
      ${SRC_FILES}
    DEPENDENCIES
      TARGET Gentau::Qml::Message
      TARGET Gentau::Qml::Bot::Common
)

target_include_directories(${BOTS_HERO_TAR}
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/inc>
)

target_link_libraries(${BOTS_HERO_TAR}
  PUBLIC
    Qt6::Core
    Qt6::Quick
    Qt6::Qml

    Gentau::ImgTrans

    Gentau::App::BotsCommon
    Gentau::App::Msg
    Gentau::App::Input
  PRIVATE
    Gentau::Utils
)