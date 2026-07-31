file(
  GLOB_RECURSE SRC_FILES
  RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}"
  CONFIGURE_DEPENDS "inc/*.hpp" "*.cpp"
)

gt_register_qml_mod(
  TARGET bots-infantry
  URI_SUFFIX Bot.Infantry
  REAL_TARGET_VAR BOTS_INFANTRY_TAR
  QT_ARGS
    STATIC
    SOURCES
      ${SRC_FILES}
    DEPENDENCIES
      TARGET Gentau::Qml::Bot::Common
)

target_include_directories(${BOTS_INFANTRY_TAR}
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/inc>
)

target_link_libraries(${BOTS_INFANTRY_TAR}
  PUBLIC
    Qt6::Core
    Qt6::Qml

    Gentau::App::BotsCommon
    Gentau::App::Msg
  PRIVATE
    Gentau::Utils
)