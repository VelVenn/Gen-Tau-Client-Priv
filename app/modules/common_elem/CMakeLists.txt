file(
  GLOB QML_FILES
  RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}"
  CONFIGURE_DEPENDS "*.qml"
)

gt_register_qml_mod(
  TARGET common-elem
  URI_SUFFIX CommonElem
  REAL_TARGET_VAR COMMON_ELEM_TAR
  QML_FILES
    ${QML_FILES}
  QT_ARGS
    STATIC
    IMPORTS
      TARGET Gentau::Qml::Foundation
    DEPENDENCIES
      TARGET Gentau::Qml::BasicWidgets
)

target_link_libraries(${COMMON_ELEM_TAR}
  PRIVATE
    Qt6::Quick
    Gentau::App::BasicWidgets
  
  PUBLIC
    Gentau::App::UiFoundation
)