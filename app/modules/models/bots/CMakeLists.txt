add_subdirectory(common)
add_subdirectory(hero)
add_subdirectory(factory)

file(
  GLOB_RECURSE HEADER_FILES
  RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}"
  CONFIGURE_DEPENDS "inc/*.hpp"
)

file(
  GLOB SRC_FILES
  RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}"
  CONFIGURE_DEPENDS "*.cpp"
)

gt_register_qml_mod(
  TARGET bots-model
  URI_SUFFIX Bot
  REAL_TARGET_VAR BOTS_MODEL_TAR
  QT_ARGS
    STATIC
    DEPENDENCIES
      TARGET Gentau::Qml::Bot::Common
    SOURCES
      ${SRC_FILES}
      ${HEADER_FILES}
)

target_include_directories(${BOTS_MODEL_TAR}
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/inc>
)

target_link_libraries(${BOTS_MODEL_TAR}
  PUBLIC
    Qt6::Core
    Qt6::Qml

    Gentau::App::BotsCommon
  PRIVATE
    Gentau::Utils
)