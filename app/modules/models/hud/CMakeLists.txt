file(
  GLOB_RECURSE INC_FILES
  RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}"
  CONFIGURE_DEPENDS "inc/*.hpp" 
)

file(
  GLOB SRC_FILES
  RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}"
  CONFIGURE_DEPENDS "*.cpp"
)

gt_register_qt_lib(
  TARGET hud-model
  NO_ALIAS
  REAL_TARGET_VAR HUD_MODEL_TAR
  QT_ARGS
    STATIC
    ${INC_FILES}
    ${SRC_FILES}
)

gt_register_qml_mod(
  TARGET hud-model
  URI_SUFFIX Model.Hud
  QT_ARGS
    DEPENDENCIES
      TARGET Gentau::Qml::Model::GlobalStatus
)

target_include_directories(${HUD_MODEL_TAR}
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/inc>
)

target_link_libraries(${HUD_MODEL_TAR}
  PUBLIC
    Qt6::Core
    Qt6::Qml

    Gentau::App::GlobalStatusModel

  PRIVATE
    Gentau::Utils
)