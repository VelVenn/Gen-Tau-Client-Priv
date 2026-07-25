file(
  GLOB_RECURSE SRC_FILES
  RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}"
  CONFIGURE_DEPENDS "inc/*.hpp" "*.cpp"
)

gt_register_qt_lib(
  TARGET base-model
  REAL_TARGET_VAR BASE_MODEL_TAR
  QT_ARGS
    STATIC
    ${SRC_FILES}
)

target_include_directories(${BASE_MODEL_TAR}
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/inc>
)

target_link_libraries(${BASE_MODEL_TAR}
  PUBLIC
    Qt6::Core

    Gentau::App::MqttAdapter
    Gentau::App::Msg
)