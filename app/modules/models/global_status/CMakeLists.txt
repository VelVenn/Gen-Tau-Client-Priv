file(
  GLOB_RECURSE SRC_FILES
  RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}"
  CONFIGURE_DEPENDS "inc/*.hpp" "*.cpp"
)

gt_register_qt_lib(
  TARGET global-status-model
  NO_ALIAS
  REAL_TARGET_VAR GLOBAL_STATUS_MODEL_TAR
  QT_ARGS
    STATIC
    ${SRC_FILES}
)

gt_register_qml_mod(
  TARGET global-status-model
  URI_SUFFIX Model.GlobalStatus
  QT_ARGS
    IMPORTS
      TARGET Gentau::Qml::Message
)

target_include_directories(${GLOBAL_STATUS_MODEL_TAR}
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/inc>
)

target_link_libraries(${GLOBAL_STATUS_MODEL_TAR}
  PUBLIC
    Qt6::Core
    Qt6::Qml

    Gentau::App::BaseModel

  PRIVATE
    Gentau::Utils
)