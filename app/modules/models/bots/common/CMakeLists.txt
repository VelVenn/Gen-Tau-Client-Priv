file(
  GLOB_RECURSE SRC_FILES
  RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}"
  CONFIGURE_DEPENDS "inc/*.hpp" "*.cpp"
)

gt_register_qt_lib(
  TARGET bots-common
  NO_ALIAS
  REAL_TARGET_VAR BOTS_COMMON_TAR
  QT_ARGS
    STATIC
    ${SRC_FILES}
)

gt_register_qml_mod(
  TARGET bots-common
  URI_SUFFIX Bot.Common
  QT_ARGS
    IMPORTS
      TARGET Gentau::Qml::Message
)

target_include_directories(${BOTS_COMMON_TAR}
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/inc>
)

target_link_libraries(${BOTS_COMMON_TAR}
  PUBLIC
    Qt6::Core
    Qt6::Qml

    Gentau::App::BaseModel

  PRIVATE
    Gentau::Utils
)